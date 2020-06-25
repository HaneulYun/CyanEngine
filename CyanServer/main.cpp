#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma comment (lib, "WS2_32.lib")
#pragma comment(lib, "mswsock.lib")
#pragma comment(lib, "lua53.lib")

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <locale.h>

#define UNICODE  
#include <sqlext.h>  

#include <vector>
#include <thread>
#include <mutex>
#include <unordered_set>
#include <atomic>
#include <chrono>
#include <queue>
using namespace std;
using namespace chrono;

#include "protocol.h"
constexpr auto MAX_PACKET_SIZE = 255;
constexpr auto MAX_BUF_SIZE = 1024;
constexpr auto MAX_USER = NPC_ID_START;

constexpr auto VIEW_RADIUS = 8;

enum ENUMOP { OP_RECV, OP_SEND, OP_ACCEPT, OP_RANDOM_MOVE, OP_PLAYER_MOVE, OP_MOVE_SELF, OP_SEND_BYE };
struct event_type
{
	int obj_id;
	ENUMOP event_id;
	high_resolution_clock::time_point wakeup_time;
	int target_id;

	constexpr bool operator < (const event_type& right) const
	{
		return (wakeup_time > right.wakeup_time);
	}
};

priority_queue<event_type> timer_queue;
mutex timer_lock;

enum C_STATUS { ST_FREE, ST_ALLOC, ST_ACTIVE, ST_SLEEP };

struct EXOVER
{
	WSAOVERLAPPED	over; // 맨 위에 overlapped 구조체가 있어야 한다.
	ENUMOP			op;
	char			io_buf[MAX_BUF_SIZE];
	union {
		WSABUF		wsabuf;
		SOCKET		c_socket;
		int			p_id;
	};
};

struct CLIENT
{
	mutex m_cl;
	SOCKET m_s;
	int m_id;
	EXOVER m_recv_over;
	int m_prev_size;
	char m_papcket_buf[MAX_PACKET_SIZE];
	atomic<C_STATUS> m_status;

	short x, y;
	char m_name[MAX_ID_LEN + 1];
	unsigned m_move_time;
	high_resolution_clock::time_point m_last_move_time;

	unordered_set<int> view_list;
	lua_State* L;
	mutex lua_l;
};

CLIENT g_clients[NPC_ID_START + NUM_NPC];
HANDLE g_iocp;
SOCKET l_socket;

void add_timer(int obj_id, ENUMOP op_type, int duration, int target_id = 0)
{
	timer_lock.lock();
	event_type ev{ obj_id, op_type, high_resolution_clock::now() + milliseconds(duration), target_id };
	timer_queue.push(ev);
	timer_lock.unlock();
}

bool is_player(int id)
{
	return id < NPC_ID_START;
}

bool is_near(int a, int b)
{
	if (abs(g_clients[a].x - g_clients[b].x) > VIEW_RADIUS) return false;
	if (abs(g_clients[a].y - g_clients[b].y) > VIEW_RADIUS) return false;
	return true;
}

void send_packet(int user_id, void* p)
{
	char* buf = reinterpret_cast<char*>(p);

	CLIENT& u = g_clients[user_id];

	EXOVER* exover = new EXOVER;
	exover->op = OP_SEND;
	ZeroMemory(&exover->over, sizeof(exover->over));
	exover->wsabuf.buf = exover->io_buf;
	exover->wsabuf.len = unsigned char(buf[0]);
	memcpy(exover->io_buf, buf, unsigned char(buf[0]));

	WSASend(u.m_s, &exover->wsabuf, 1, NULL, 0, &exover->over, NULL);
}

void send_login_ok_packet(int user_id)
{
	sc_packet_login_ok p;
	p.exp = 0;
	p.hp = 0;
	p.id = user_id;
	p.level = 0;
	p.size = sizeof(p);
	p.type = S2C_LOGIN_OK;
	p.x = g_clients[user_id].x;
	p.y = g_clients[user_id].y;

	send_packet(user_id, &p);
}

void send_enter_packet(int user_id, int o_id)
{
	sc_packet_enter p;
	p.id = o_id;
	p.size = sizeof(p);
	p.type = S2C_ENTER;
	p.x = g_clients[o_id].x;
	p.y = g_clients[o_id].y;
	strcpy_s(p.name, g_clients[o_id].m_name);
	p.o_type = O_HUMAN;

	g_clients[user_id].m_cl.lock();
	g_clients[user_id].view_list.insert(o_id);
	g_clients[user_id].m_cl.unlock();

	send_packet(user_id, &p);
}

void send_leave_packet(int user_id, int o_id)
{
	sc_packet_leave p;
	p.id = o_id;
	p.size = sizeof(p);
	p.type = S2C_LEAVE;

	g_clients[user_id].m_cl.lock();
	g_clients[user_id].view_list.erase(o_id);
	g_clients[user_id].m_cl.unlock();

	send_packet(user_id, &p);
}

void send_move_packet(int user_id, int mover)
{
	sc_packet_move p;
	p.id = mover;
	p.size = sizeof(p);
	p.type = S2C_MOVE;
	p.x = g_clients[mover].x;
	p.y = g_clients[mover].y;
	p.move_time = g_clients[mover].m_move_time;

	send_packet(user_id, &p);
}

void send_chat_packet(int user_id, int chatter, const wchar_t mess[])
{
	sc_packet_chat p;
	p.id = chatter;
	p.size = sizeof(p);
	p.type = S2C_CHAT;
	wcscpy(p.mess, mess);

	send_packet(user_id, &p);
}

void activate_npc(int id)
{
	C_STATUS old_state = ST_SLEEP;
	if (true == atomic_compare_exchange_strong(&g_clients[id].m_status, &old_state, ST_ACTIVE))
		add_timer(id, OP_MOVE_SELF, 1000);
}

void do_move(int user_id, int direction)
{
	CLIENT& u = g_clients[user_id];
	int x = u.x;
	int y = u.y;
	switch (direction)
	{
	case D_UP: if (y < (WORLD_HEIGHT - 1)) y++;
		break;
	case D_DOWN: if (y > 0) y--;
		break;
	case D_LEFT: if (x > 0) x--;
		break;
	case D_RIGHT: if (x < (WORLD_WIDTH - 1)) x++;
		break;
	default:
		cout << "Unknown Direction from Client move packet!\n";
		DebugBreak();
		exit(-1);
	}
	u.x = x;
	u.y = y;

	g_clients[user_id].m_cl.lock();
	unordered_set<int> old_vl = g_clients[user_id].view_list;
	g_clients[user_id].m_cl.unlock();
	unordered_set<int> new_vl;
	for (auto& cl : g_clients)
	{
		if (false == is_near(cl.m_id, user_id)) continue;
		if (ST_SLEEP == cl.m_status) activate_npc(cl.m_id);
		if (ST_ACTIVE != cl.m_status) continue;
		if (cl.m_id == user_id) continue;
		if (false == is_player(cl.m_id))
		{
			EXOVER* over = new EXOVER;
			over->op = OP_PLAYER_MOVE;
			over->p_id = user_id;
			PostQueuedCompletionStatus(g_iocp, 1, cl.m_id, &over->over);
		}
		new_vl.insert(cl.m_id);
	}

	send_move_packet(user_id, user_id);

	for (auto np : new_vl) {
		if (0 == old_vl.count(np)) {
			send_enter_packet(user_id, np);
			if (false == is_player(np))
				continue;
			g_clients[np].m_cl.lock();
			if (0 == g_clients[np].view_list.count(user_id))
			{
				g_clients[np].m_cl.unlock();
				send_enter_packet(np, user_id);
			}
			else
			{
				g_clients[np].m_cl.unlock();
				send_move_packet(np, user_id);
			}
		}
		else
		{
			if (false == is_player(np)) continue;
			g_clients[np].m_cl.lock();
			if (0 != g_clients[np].view_list.count(user_id))
			{
				g_clients[np].m_cl.unlock();
				send_move_packet(np, user_id);
			}
			else
			{
				g_clients[np].m_cl.unlock();
				send_enter_packet(np, user_id);
			}
		}
	}

	for (auto old_p : old_vl)
	{
		if (0 == new_vl.count(old_p))
		{
			send_leave_packet(user_id, old_p);
			if (false == is_player(old_p)) continue;
			g_clients[old_p].m_cl.lock();
			if (0 != g_clients[old_p].view_list.count(user_id))
			{
				g_clients[old_p].m_cl.unlock();
				send_leave_packet(old_p, user_id);
			}
			else
				g_clients[old_p].m_cl.unlock();
		}
	}

	//for (auto& cl : g_clients)
	//{
	//	cl.m_cl.lock();
	//	if (ST_ACTIVE == cl.m_status)
	//		send_move_packet(cl.m_id, user_id);
	//	cl.m_cl.unlock();
	//}
}

void random_move_npc(int id)
{
	int x = g_clients[id].x;
	int y = g_clients[id].y;
	switch (rand() % 4)
	{
	case 0: if (x < (WORLD_WIDTH - 1)) ++x; break;
	case 1: if (x > 0) --x; break;
	case 2: if (y < (WORLD_HEIGHT - 1)) ++y; break;
	case 3: if (y > 0) --y; break;
	}
	g_clients[id].x = x;
	g_clients[id].y = y;
	for (int i = 0; i < NPC_ID_START; ++i)
	{
		if (ST_ACTIVE != g_clients[i].m_status) continue;
		if (true == is_near(i, id))
		{
			g_clients[i].m_cl.lock();
			if (0 != g_clients[i].view_list.count(id))
			{
				g_clients[i].m_cl.unlock();
				send_move_packet(i, id);
			}
			else
			{
				g_clients[i].m_cl.unlock();
				send_enter_packet(i, id);
			}
		}
		else
		{
			g_clients[i].m_cl.lock();
			if (0 != g_clients[i].view_list.count(id))
			{
				g_clients[i].m_cl.unlock();
				send_leave_packet(i, id);
			}
			else
				g_clients[i].m_cl.unlock();
		}
	}
}

void show_error() {
	printf("error\n");
}

/************************************************************************
/* HandleDiagnosticRecord : display error/warning information
/*
/* Parameters:
/* hHandle ODBC handle
/* hType Type of handle (SQL_HANDLE_STMT, SQL_HANDLE_ENV, SQL_HANDLE_DBC)
/* RetCode Return code of failing command
/************************************************************************/
void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
	SQLSMALLINT iRec = 0;
	SQLINTEGER iError;
	WCHAR wszMessage[1000];
	WCHAR wszState[SQL_SQLSTATE_SIZE + 1];
	if (RetCode == SQL_INVALID_HANDLE) {
		fwprintf(stderr, L"Invalid handle!\n");
		return;
	}
	while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage,
		(SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT*)NULL) == SQL_SUCCESS) {
		// Hide data truncated..
		if (wcsncmp(wszState, L"01004", 5)) {
			fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
		}
	}
}

void enter_game(int user_id, char name[])
{
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;

	SQLWCHAR data_id[20];
	SQLINTEGER data_x, data_y;

	SQLLEN cbid = 0, cbx = 0, cby = 0;

	setlocale(LC_ALL, "korean");

	// Allocate environment handle  
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	bool found{ false };
	// Set the ODBC version environment attribute  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		// Allocate connection handle  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			// Set login timeout to 5 seconds  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

				// Connect to data source  
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"game_db_odbc_2017182027", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

				// Allocate statement handle  
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					printf("ODBC connect OK!\n");
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

					wstring n(name, &name[strlen(name)]);
					wstring exec = L"EXEC select_id " + n;
					retcode = SQLExecDirect(hstmt, (SQLWCHAR*)exec.c_str(), SQL_NTS);
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						printf("Select OK\n");
						found = true;
						// Bind columns 1, 2, and 3  
						retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, &data_id, 20, &cbid);
						retcode = SQLBindCol(hstmt, 2, SQL_C_LONG, &data_x, 100, &cbx);
						retcode = SQLBindCol(hstmt, 3, SQL_C_LONG, &data_y, 100, &cby);

						// Fetch and print each row of data. On an error, display a message and exit.  
						for (int i = 0; ; i++) {
							retcode = SQLFetch(hstmt);
							if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
								show_error();
							if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
							{
								wprintf(L"%d: %ls %d %d\n", i + 1, data_id, data_x, data_y);
							}
							else
								break;
						}
					}

					// Process data  
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						SQLCancel(hstmt);
						SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
					}

					SQLDisconnect(hdbc);
				}
				else
					HandleDiagnosticRecord(hdbc, SQL_HANDLE_DBC, retcode);

				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}

	if (!found)
		return;

	g_clients[user_id].m_cl.lock();
	strcpy(g_clients[user_id].m_name, name);
	g_clients[user_id].m_name[MAX_ID_LEN] = NULL;
	g_clients[user_id].x = data_x;
	g_clients[user_id].y = data_y;
	send_login_ok_packet(user_id);
	g_clients[user_id].m_status = ST_ACTIVE;
	g_clients[user_id].m_cl.unlock();

	for (auto& cl : g_clients)
	{
		int i = cl.m_id;
		if (user_id == i) continue;
		if (true == is_near(user_id, i))
		{
			//g_clients[i].m_cl.lock();
			if (ST_SLEEP == g_clients[i].m_status)
				activate_npc(i);
			if (ST_ACTIVE == g_clients[i].m_status)
			{
				send_enter_packet(user_id, i);
				if (true == is_player(i))
					send_enter_packet(i, user_id);
			}
			//g_clients[i].m_cl.unlock();
		}
	}

}

void process_packet(int user_id, char* buf)
{
	switch (buf[1])
	{
	case C2S_LOGIN:
	{
		cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(buf);
		enter_game(user_id, packet->name);
	}
	break;
	case C2S_MOVE:
	{
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(buf);
		g_clients[user_id].m_move_time = packet->move_time;
		do_move(user_id, packet->direction);
	}
	break;
	default:
		cout << "Unknown Packet Type Error!\n";
		DebugBreak(); // 여기서 멈추게 된다.
		exit(-1);
	}
}

void initialize_clients()
{
	for (int i = 0; i < MAX_USER; ++i)
	{
		g_clients[i].m_id = i;
		g_clients[i].m_status = ST_FREE;
	}
}

void disconnect(int user_id)
{
	send_leave_packet(user_id, user_id);

	g_clients[user_id].m_cl.lock();
	g_clients[user_id].m_status = ST_ALLOC;

	closesocket(g_clients[user_id].m_s);
	for (int i = 0; i < NPC_ID_START; ++i)
	{
		CLIENT& cl = g_clients[i];
		if (user_id == cl.m_id) continue;
		//cl.m_cl.lock();
		if (ST_ACTIVE == cl.m_status)
			send_leave_packet(cl.m_id, user_id);
		//cl.m_cl.unlock();
	}
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;

	SQLWCHAR data_id[20];
	SQLINTEGER data_x, data_y;

	SQLLEN cbid = 0, cbx = 0, cby = 0;

	setlocale(LC_ALL, "korean");

	// Allocate environment handle  
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	// Set the ODBC version environment attribute  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		// Allocate connection handle  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			// Set login timeout to 5 seconds  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

				// Connect to data source  
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"game_db_odbc_2017182027", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

				// Allocate statement handle  
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					printf("ODBC connect OK!\n");
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

					string nn = g_clients[user_id].m_name;
					wstring n(nn.begin(), nn.end());
					wstring exec = L"EXEC update_id " + n + L", ";
					exec += to_wstring(g_clients[user_id].x);
					exec += L", ";
					exec += to_wstring(g_clients[user_id].y);
					retcode = SQLExecDirect(hstmt, (SQLWCHAR*)exec.c_str(), SQL_NTS);
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						printf("Update OK\n");
					}

					// Process data  
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						SQLCancel(hstmt);
						SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
					}

					SQLDisconnect(hdbc);
				}
				else
					HandleDiagnosticRecord(hdbc, SQL_HANDLE_DBC, retcode);

				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}

	g_clients[user_id].m_status = ST_FREE;
	g_clients[user_id].m_cl.unlock();
}

void recv_packet_construct(int user_id, int io_byte)
{
	CLIENT& cu = g_clients[user_id];
	EXOVER& r_o = cu.m_recv_over;

	int rest_byte = io_byte;
	char* p = r_o.io_buf;
	int packet_size = 0;
	if (0 != cu.m_prev_size)
		packet_size = cu.m_papcket_buf[0];
	while (rest_byte)
	{
		if (0 == packet_size)
			packet_size = *p;
		if (packet_size <= rest_byte + cu.m_prev_size)
		{
			memcpy(cu.m_papcket_buf + cu.m_prev_size, p, packet_size - cu.m_prev_size);
			p += packet_size - cu.m_prev_size;
			rest_byte -= packet_size - cu.m_prev_size;
			packet_size = 0;
			process_packet(user_id, cu.m_papcket_buf);
			cu.m_prev_size = 0;
		}
		else
		{
			memcpy(cu.m_papcket_buf + cu.m_prev_size, p, rest_byte);
			cu.m_prev_size += rest_byte;
			rest_byte = 0;
			p += rest_byte;
		}
	}
}

void worker_thread()
{
	while (true)
	{
		DWORD io_byte;
		ULONG_PTR key;
		WSAOVERLAPPED* over;
		GetQueuedCompletionStatus(g_iocp, &io_byte, &key, &over, INFINITE);

		EXOVER* exover = reinterpret_cast<EXOVER*>(over);
		int user_id = static_cast<int>(key);
		CLIENT& cl = g_clients[user_id];

		switch (exover->op)
		{
		case OP_RECV:
		{
			if (!io_byte)
				disconnect(user_id);
			else
			{
				recv_packet_construct(user_id, io_byte);

				ZeroMemory(&cl.m_recv_over.over, 0, sizeof(cl.m_recv_over.over));
				DWORD flags = 0;
				WSARecv(cl.m_s, &cl.m_recv_over.wsabuf, 1, NULL, &flags, &cl.m_recv_over.over, NULL);
			}
		}
		break;
		case OP_SEND:
			if (!io_byte)
				disconnect(user_id);
			delete exover;
			break;
		case OP_ACCEPT:
		{
			int user_id = -1;
			for (int i = 0; i < MAX_USER; ++i)
			{
				lock_guard<mutex> gl{ g_clients[i].m_cl };
				if (ST_FREE == g_clients[i].m_status)
				{
					g_clients[i].m_status = ST_ALLOC;
					user_id = i;
					break;
				}
			}

			SOCKET c_socket = exover->c_socket;
			if (-1 == user_id)
				closesocket(c_socket);
			else
			{
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket), g_iocp, user_id, 0);

				CLIENT& nc = g_clients[user_id];
				nc.m_prev_size = 0;
				nc.m_recv_over.op = OP_RECV;
				ZeroMemory(&nc.m_recv_over.over, 0, sizeof(nc.m_recv_over.over));
				nc.m_recv_over.wsabuf.buf = nc.m_recv_over.io_buf;
				nc.m_recv_over.wsabuf.len = MAX_BUF_SIZE;
				nc.m_s = c_socket;
				nc.view_list.clear();
				nc.x = rand() % WORLD_WIDTH;
				nc.y = rand() % WORLD_HEIGHT;

				DWORD flags = 0;
				WSARecv(c_socket, &nc.m_recv_over.wsabuf, 1, NULL, &flags, &nc.m_recv_over.over, NULL);
			}

			c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			exover->c_socket = c_socket;
			ZeroMemory(&exover->over, sizeof(exover->over));
			AcceptEx(l_socket, c_socket, exover->io_buf, NULL, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &exover->over);
		}
		break;
		case OP_RANDOM_MOVE:
		{
			random_move_npc(user_id);
			bool keep_alive = false;
			for (int i = 0; i < NPC_ID_START; ++i)
				if (true == is_near(user_id, i))
					if (ST_ACTIVE == g_clients[i].m_status)
					{
						keep_alive = true;
						break;
					}
			if (true == keep_alive) add_timer(user_id, OP_RANDOM_MOVE, 1000);
			else g_clients[user_id].m_status = ST_SLEEP;
			delete exover;
		}
		break;
		case OP_PLAYER_MOVE:
		{
			g_clients[user_id].lua_l.lock();
			lua_State* L = g_clients[user_id].L;
			lua_getglobal(L, "event_player_move");
			lua_pushnumber(L, exover->p_id);
			int error = lua_pcall(L, 1, 0, 0);
			if (error) cout << lua_tostring(L, -1);
			g_clients[user_id].lua_l.unlock();
			delete exover;
		}
		break;
		case OP_MOVE_SELF:
		{
			g_clients[user_id].lua_l.lock();
			lua_State* L = g_clients[user_id].L;
			lua_getglobal(L, "event_npc_move");
			lua_pushnumber(L, user_id);
			int error = lua_pcall(L, 1, 0, 0);
			if (error) cout << lua_tostring(L, -1);
			g_clients[user_id].lua_l.unlock();

			bool keep_alive = false;
			for (int i = 0; i < NPC_ID_START; ++i)
				if (true == is_near(user_id, i))
					if (ST_ACTIVE == g_clients[i].m_status)
					{
						keep_alive = true;
						break;
					}
			if (true == keep_alive) add_timer(user_id, OP_MOVE_SELF, 1000);
			else g_clients[user_id].m_status = ST_SLEEP;
			delete exover;
		}
		break;
		case OP_SEND_BYE:
		{
			wstring mess = L"BYE";
			send_chat_packet(user_id, exover->p_id, mess.c_str());
			delete exover;
		}
		break;
		default:
			cout << "Unknown Operation in worker_thread!!!\n";
			while (true);
		}
	}
}

int API_RandomMove(lua_State* L)
{
	int o_id = (int)lua_tointeger(L, -1);
	random_move_npc(o_id);
	lua_pop(L, 1);
	return 0;
}

int API_PushQueueByeMessage(lua_State* L)
{
	int my_id = (int)lua_tointeger(L, -2);
	int user_id = (int)lua_tointeger(L, -1);

	add_timer(user_id, OP_SEND_BYE, 3000, my_id);
	lua_pop(L, 2);
	return 0;
}

int API_SendMessage(lua_State* L)
{
	int my_id = (int)lua_tointeger(L, -3);
	int user_id = (int)lua_tointeger(L, -2);
	char* mess = (char*)lua_tostring(L, -1);
	wstring w_mess(mess, &mess[strlen(mess)]);

	send_chat_packet(user_id, my_id, w_mess.c_str());
	lua_pop(L, 3);
	return 0;
}

int API_get_x(lua_State* L)
{
	int obj_id = (int)lua_tointeger(L, -1);
	lua_pop(L, 2);
	int x = g_clients[obj_id].x;
	lua_pushnumber(L, x);
	return 1;
}

int API_get_y(lua_State* L)
{
	int obj_id = (int)lua_tointeger(L, -1);
	lua_pop(L, 2);
	int y = g_clients[obj_id].y;
	lua_pushnumber(L, y);
	return 1;
}

void init_npc()
{
	for (int i = NPC_ID_START; i < NPC_ID_START + NUM_NPC; ++i)
	{
		g_clients[i].m_s = 0;
		g_clients[i].m_id = i;
		sprintf(g_clients[i].m_name, "NPC%d", i);
		g_clients[i].m_status = ST_SLEEP;
		g_clients[i].x = rand() % WORLD_WIDTH;
		g_clients[i].y = rand() % WORLD_HEIGHT;
		//g_clients[i].m_last_move_time = high_resolution_clock::now();
		//add_timer(i, OP_RANDOM_MOVE, 1000);
		lua_State* L = g_clients[i].L = luaL_newstate();
		luaL_openlibs(L);
		luaL_loadfile(L, "NPC.LUA");
		lua_pcall(L, 0, 0, 0);
		lua_getglobal(L, "set_uid");
		lua_pushnumber(L, i);
		lua_pcall(L, 1, 0, 0);
		lua_pop(L, 1);

		lua_register(L, "API_random_move", API_RandomMove);
		lua_register(L, "API_push_queue_bye_message", API_PushQueueByeMessage);
		lua_register(L, "API_send_message", API_SendMessage);
		lua_register(L, "API_get_x", API_get_x);
		lua_register(L, "API_get_y", API_get_y);
	}
}

void do_ai()
{
	while (true)
	{
		auto ai_start_time = high_resolution_clock::now();
		for (int i = NPC_ID_START; i < NPC_ID_START + NUM_NPC; ++i)
		{
			if ((high_resolution_clock::now() - g_clients[i].m_last_move_time) > 1s)
			{
				random_move_npc(i);
				g_clients[i].m_last_move_time = high_resolution_clock::now();
			}
		}
		auto ai_time = high_resolution_clock::now() - ai_start_time;
		cout << "AI Exec Time = " << duration_cast<milliseconds>(ai_time).count() << "ms\n";
	}
}

void do_timer()
{
	while (true)
	{
		this_thread::sleep_for(1ms);
		while (true)
		{
			timer_lock.lock();
			if (true == timer_queue.empty())
			{
				timer_lock.unlock();
				break;
			}
			if (timer_queue.top().wakeup_time > high_resolution_clock::now())
			{
				timer_lock.unlock();
				break;
			}
			event_type ev = timer_queue.top();
			timer_queue.pop();
			timer_lock.unlock();
			switch (ev.event_id)
			{
			case OP_MOVE_SELF:
			{
				EXOVER* over = new EXOVER;
				over->op = ev.event_id;
				PostQueuedCompletionStatus(g_iocp, 1, ev.obj_id, &over->over);
				//random_move_npc(ev.obj_id);
				//add_timer(ev.obj_id, ev.event_id, 1000);
			}
			break;
			case OP_SEND_BYE:
			{
				EXOVER* over = new EXOVER;
				over->op = ev.event_id;
				over->p_id = ev.target_id;
				PostQueuedCompletionStatus(g_iocp, 1, ev.obj_id, &over->over);
			}
			break;
			}
		}
	}
}

int main()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	cout << "NPC Initialize start.\n";
	init_npc();
	cout << "NPC Initialize finished\n";

	l_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN s_address;
	memset(&s_address, 0, sizeof(s_address));
	s_address.sin_family = AF_INET;
	s_address.sin_port = htons(SERVER_PORT); // 그냥 넣어주면 안돼요.
	s_address.sin_addr.S_un.S_addr = htons(INADDR_ANY);
	::bind(l_socket, reinterpret_cast<sockaddr*>(&s_address), sizeof(s_address));
	// bind 를 그냥 쓰면 안되죠.
	// 그냥 바인드를 쓰면 c++11 에 있는 바인드 키워드와 연결이 돼요.
	// C++ 에 있는 바인드를 쓰지 않으려면, :: 를 해주어야 한다.

	listen(l_socket, SOMAXCONN);

	g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	initialize_clients();

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(l_socket), g_iocp, 999, 0);
	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	EXOVER accept_over;
	ZeroMemory(&accept_over.over, sizeof(accept_over.over));
	accept_over.op = OP_ACCEPT;
	accept_over.c_socket = c_socket;
	AcceptEx(l_socket, c_socket, accept_over.io_buf, NULL, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &accept_over.over);

	vector<thread> worker_threads;
	for (int i = 0; i < 4; ++i) worker_threads.emplace_back(worker_thread);

	//thread ai_thread{ do_ai };
	//ai_thread.join();

	thread timer_thread{ do_timer };

	for (auto& th : worker_threads) th.join();
}