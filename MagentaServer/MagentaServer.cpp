#include <iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "mswsock.lib")
#pragma comment(lib, "lua53.lib")

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <vector>
#include <thread>
#include <unordered_set>
#include <atomic>
#include <chrono>
#include <queue>
#include <string>

#include <Windows.h>
#include <locale.h>

#define UNICODE
#include <sqlext.h>

using namespace std;
using namespace chrono;

#include "protocol.h"
constexpr auto MAX_PACKET_SIZE = 255;
constexpr auto MAX_BUF_SIZE = 1024;
constexpr auto MAX_USER = 10000;

constexpr auto VIEW_RADIUS = 8;

enum ENUMOP { OP_RECV, OP_SEND, OP_ACCEPT, OP_RANDOM_MOVE, OP_PLAYER_MOVE, OP_RUN, OP_RUN_FINISH };

class RWLock
{
public:
	RWLock() {}
	~RWLock() {}

	RWLock(const RWLock& rhs) = delete;
	RWLock& operator=(const RWLock& rhs) = delete;

	void EnterWriteLock()
	{
		while (true)
		{
			while (mLockFlag & LF_WRITE_MASK)
				YieldProcessor();
			if ((InterlockedAdd(&mLockFlag, LF_WRITE_FLAG) & LF_WRITE_MASK) == LF_WRITE_FLAG)
			{
				while (mLockFlag & LF_READ_MASK)
					YieldProcessor();

				return;
			}
			InterlockedAdd(&mLockFlag, -LF_WRITE_FLAG);
		}
	}
	void LeaveWriteLock()
	{
		InterlockedAdd(&mLockFlag, -LF_WRITE_FLAG);
	}

	void EnterReadLock()
	{
		while (true)
		{
			while (mLockFlag & LF_WRITE_MASK)
				YieldProcessor();

			if ((InterlockedIncrement(&mLockFlag) & LF_WRITE_MASK) == 0)
				return;
			else
				InterlockedDecrement(&mLockFlag);
		}
	}
	void LeaveReadLock()
	{
		InterlockedDecrement(&mLockFlag);
	}

	long GetLockFlag() const { return mLockFlag; }

private:
	enum LockFlag
	{
		LF_WRITE_MASK = 0x7FF00000,
		LF_WRITE_FLAG = 0x00100000,
		LF_READ_MASK = 0x000FFFFF
	};
	volatile long mLockFlag;
};

#define SECTOR_WIDTH 20

unordered_set<int> g_ObjectListSector[WORLD_HEIGHT / SECTOR_WIDTH][WORLD_WIDTH / SECTOR_WIDTH];
RWLock g_SectorLock[WORLD_HEIGHT / SECTOR_WIDTH][WORLD_WIDTH / SECTOR_WIDTH];

struct event_type {
	int obj_id;
	ENUMOP event_id;
	high_resolution_clock::time_point wakeup_time;
	int target_id;

	constexpr bool operator <(const event_type& left) const
	{
		return (wakeup_time > left.wakeup_time);
	}
};

priority_queue<event_type> timer_queue;	// 포인터로 선언해서 new delete 하라
RWLock timer_lock;

enum C_STATUS { ST_FREE, ST_ALLOC, ST_ACTIVE, ST_SLEEP };

struct EXOVER {
	WSAOVERLAPPED	over;
	ENUMOP			op;
	char			io_buf[MAX_BUF_SIZE];
	union {
		WSABUF			wsabuf;
		SOCKET			c_socket;
		int				p_id;
	};
};

struct Point {
	short x, y;
};

struct CLIENT {
	RWLock	m_cl;
	SOCKET	m_s;
	int		m_id;	// 한번 세팅하면 바꾸지 않으니 mutex 할 필요 없다
	EXOVER	m_recv_over;
	int		m_prev_size;
	char	m_packet_buf[MAX_PACKET_SIZE];
	atomic <C_STATUS> m_status;

	short	x, y;
	char	m_name[MAX_ID_LEN + 1];
	unsigned m_move_time;
	high_resolution_clock::time_point m_last_move_time;

	unordered_set<int> view_list;
	lua_State* L;
	RWLock lua_l;
};

CLIENT g_clients[NPC_ID_START + NUM_NPC];
HANDLE g_iocp;
SOCKET l_socket;	// 한번 정해진 다음에 바뀌지 않으니 data race 발생하지 않음

void disconnect(int);

/************************************************************************
/* HandleDiagnosticRecord : display error/warning information
/*
/* Parameters:
/*      hHandle ODBC handle
/*      hType Type of handle (SQL_HANDLE_STMT, SQL_HANDLE_ENV, SQL_HANDLE_DBC)
/*      RetCode Return code of failing command
/************************************************************************/
void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
	SQLSMALLINT iRec = 0;
	SQLINTEGER  iError;
	WCHAR       wszMessage[1000];
	WCHAR       wszState[SQL_SQLSTATE_SIZE + 1];

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

void show_error() {
	printf("error\n");
}

Point find_name_in_database(string name)
{
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;
	SQLWCHAR szUser_name[MAX_ID_LEN];
	SQLINTEGER dUser_xpos, dUser_ypos;
	SQLLEN cbName = 0, cbXpos = 0, cbYpos = 0;

	wstring wname;
	wname.assign(name.begin(), name.end());
	Point p{ -1, -1 };

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
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"game_db_odbc_2017182031", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

				// Allocate statement handle  
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					printf("ODBC connect OK!\n");
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

					wstring tmp = L"EXEC select_samename " + wname;
					retcode = SQLExecDirect(hstmt, (SQLWCHAR*)tmp.c_str(), SQL_NTS);
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						printf("Select OK\n");
						// Bind columns 1, 2, and 3  
						retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, szUser_name, MAX_ID_LEN, &cbName);
						retcode = SQLBindCol(hstmt, 2, SQL_C_LONG, &dUser_xpos, 100, &cbXpos);
						retcode = SQLBindCol(hstmt, 3, SQL_C_LONG, &dUser_ypos, 100, &cbYpos);

						// Fetch and print each row of data. On an error, display a message and exit.  
						for (int i = 0; ; i++) {
							retcode = SQLFetch(hstmt);
							if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
								show_error();
							if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
							{
								//replace wprintf with printf
								//%S with %ls
								//warning C4477: 'wprintf' : format string '%S' requires an argument of type 'char *'
								//but variadic argument 2 has type 'SQLWCHAR *'
								//wprintf(L"%d: %S %S %S\n", i + 1, sCustID, szName, szPhone);  
								wprintf(L"%d: %s %d %d\n", i + 1, szUser_name, dUser_xpos, dUser_ypos);
								p.x = dUser_xpos;
								p.y = dUser_ypos;
							}
							else
							{
								break;
							}
						}
					}

					// Process data  
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						SQLCancel(hstmt);
						SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
					}

					SQLDisconnect(hdbc);
				}
				else HandleDiagnosticRecord(hdbc, SQL_HANDLE_DBC, retcode);
				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}
	return p;
}

void set_pos_in_database(string name, int x, int y)
{
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;
	SQLWCHAR szUser_name[MAX_ID_LEN];
	SQLINTEGER dUser_xpos, dUser_ypos;
	SQLLEN cbName = 0, cbXpos = 0, cbYpos = 0;

	wstring wname;
	wname.assign(name.begin(), name.end());

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
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"game_db_odbc_2017182031", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

				// Allocate statement handle  
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					printf("ODBC connect OK!\n");
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

					wstring tmp = L"EXEC set_userdata " + wname + L", " + to_wstring(x) + L", " + to_wstring(y);
					retcode = SQLExecDirect(hstmt, (SQLWCHAR*)tmp.c_str(), SQL_NTS);
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						printf("Select OK\n");
						// Bind columns 1, 2, and 3  
						retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, szUser_name, MAX_ID_LEN, &cbName);
						retcode = SQLBindCol(hstmt, 2, SQL_C_LONG, &dUser_xpos, 100, &cbXpos);
						retcode = SQLBindCol(hstmt, 3, SQL_C_LONG, &dUser_ypos, 100, &cbYpos);

						// Fetch and print each row of data. On an error, display a message and exit.  
						for (int i = 0; ; i++) {
							retcode = SQLFetch(hstmt);
							if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
								show_error();
							if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
							{
								//replace wprintf with printf
								//%S with %ls
								//warning C4477: 'wprintf' : format string '%S' requires an argument of type 'char *'
								//but variadic argument 2 has type 'SQLWCHAR *'
								//wprintf(L"%d: %S %S %S\n", i + 1, sCustID, szName, szPhone);  
								wprintf(L"%d: %s %d %d\n", i + 1, szUser_name, dUser_xpos, dUser_ypos);
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
				else HandleDiagnosticRecord(hdbc, SQL_HANDLE_DBC, retcode);
				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}
}

void add_timer(int obj_id, ENUMOP op_type, int duration, int tg_id)
{
	timer_lock.EnterWriteLock();
	event_type ev{ obj_id, op_type, high_resolution_clock::now() + milliseconds(duration), tg_id };
	timer_queue.push(ev);
	timer_lock.LeaveWriteLock();
}

bool is_player(int id)
{
	return id < NPC_ID_START;
}

bool is_near(int a, int b)
{
	if (abs(g_clients[a].x - g_clients[b].x) > VIEW_RADIUS)	return false;
	if (abs(g_clients[a].y - g_clients[b].y) > VIEW_RADIUS)	return false;
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
	exover->wsabuf.len = buf[0];
	memcpy(exover->io_buf, buf, buf[0]);
	// IpBuffers 항목에 u의 wsabuf은 이미 Recv에서 쓰고 있기 때문에 사용하면 안됨
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

	g_clients[user_id].m_cl.EnterWriteLock();
	g_clients[user_id].view_list.insert(o_id);
	g_clients[user_id].m_cl.LeaveWriteLock();

	send_packet(user_id, &p);
}

void send_leave_packet(int user_id, int o_id)
{
	sc_packet_leave p;
	p.id = o_id;
	p.size = sizeof(p);
	p.type = S2C_LEAVE;

	g_clients[user_id].m_cl.EnterWriteLock();
	g_clients[user_id].view_list.erase(o_id);
	g_clients[user_id].m_cl.LeaveWriteLock();

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

void send_chat_packet(int user_id, int chatter, char mess[])
{
	sc_packet_chat p;
	p.id = chatter;
	p.size = sizeof(p);
	p.type = S2C_CHAT;
	strcpy_s(p.mess, mess);

	send_packet(user_id, &p);
}

void activate_npc(int id)
{
	C_STATUS old_state = ST_SLEEP;
	if (true == atomic_compare_exchange_strong(&g_clients[id].m_status, &old_state, ST_ACTIVE))
		add_timer(id, OP_RANDOM_MOVE, 1000, 0);
}

void do_move(int user_id, int direction)
{
	CLIENT& u = g_clients[user_id];
	int x = u.x;
	int y = u.y;
	switch (direction)
	{
	case D_UP:	if (y > 0)	y--;	break;
	case D_DOWN:if (y < (WORLD_HEIGHT - 1))	y++;	break;
	case D_LEFT:if (x > 0)	x--;	break;
	case D_RIGHT:if (x < (WORLD_WIDTH - 1)) x++;	break;
	default:
		cout << "Unknown Direction from Client move packet!\n";
		DebugBreak();
		exit(-1);
	}

	if (u.x / SECTOR_WIDTH != x / SECTOR_WIDTH || u.y / SECTOR_WIDTH != y / SECTOR_WIDTH)
	{
		g_SectorLock[u.y / SECTOR_WIDTH][u.x / SECTOR_WIDTH].EnterWriteLock();
		g_ObjectListSector[u.y / SECTOR_WIDTH][u.x / SECTOR_WIDTH].erase(user_id);
		g_SectorLock[u.y / SECTOR_WIDTH][u.x / SECTOR_WIDTH].LeaveWriteLock();
		g_SectorLock[y / SECTOR_WIDTH][x / SECTOR_WIDTH].EnterWriteLock();
		g_ObjectListSector[y / SECTOR_WIDTH][x / SECTOR_WIDTH].insert(user_id);
		g_SectorLock[y / SECTOR_WIDTH][x / SECTOR_WIDTH].LeaveWriteLock();
	}

	u.x = x;
	u.y = y;

	g_clients[user_id].m_cl.EnterReadLock();
	unordered_set<int> old_vl = g_clients[user_id].view_list;
	g_clients[user_id].m_cl.LeaveReadLock();
	unordered_set<int> new_vl;

	for (int i = u.y / SECTOR_WIDTH - 1; i <= u.y / SECTOR_WIDTH + 1; ++i) {
		if (i < 0 || i > WORLD_HEIGHT / SECTOR_WIDTH - 1) continue;
		for (int j = u.x / SECTOR_WIDTH - 1; j <= u.x / SECTOR_WIDTH + 1; ++j) {
			if (j < 0 || j > WORLD_WIDTH / SECTOR_WIDTH - 1) continue;
			g_SectorLock[i][j].EnterReadLock();
			for (auto nearObj : g_ObjectListSector[i][j]) {
				if (false == is_near(nearObj, user_id))	continue;
				if (ST_SLEEP == g_clients[nearObj].m_status) activate_npc(nearObj);
				if (ST_ACTIVE != g_clients[nearObj].m_status)continue;
				if (nearObj == user_id)continue;
				if (false == is_player(nearObj)) {
					EXOVER* over = new EXOVER;
					over->op = OP_PLAYER_MOVE;
					over->p_id = user_id;
					PostQueuedCompletionStatus(g_iocp, 1, nearObj, &over->over);
				}
				new_vl.insert(nearObj);
			}
			g_SectorLock[i][j].LeaveReadLock();
		}
	}

	send_move_packet(user_id, user_id);

	for (auto np : new_vl) {
		if (0 == old_vl.count(np)) {	// Object가 새로 시야에 들어왔을 때
			send_enter_packet(user_id, np);
			if (false == is_player(np))
				continue;
			g_clients[np].m_cl.EnterReadLock();
			if (0 == g_clients[np].view_list.count(user_id)) {
				g_clients[np].m_cl.LeaveReadLock();
				send_enter_packet(np, user_id);
			}
			else {
				g_clients[np].m_cl.LeaveReadLock();
				send_move_packet(np, user_id);
			}
		}
		else {	// 계속 시야에 존재하고 있을 때
			if (false == is_player(np))	continue;
			g_clients[np].m_cl.EnterReadLock();
			if (0 != g_clients[np].view_list.count(user_id)) {
				g_clients[np].m_cl.LeaveReadLock();
				send_move_packet(np, user_id);
			}
			else {
				g_clients[np].m_cl.LeaveReadLock();
				send_enter_packet(np, user_id);
			}
		}
	}

	for (auto old_p : old_vl) {		// Object가 시야에서 벗어났을 때
		if (0 == new_vl.count(old_p)) {
			send_leave_packet(user_id, old_p);
			if (false == is_player(old_p))	continue;
			g_clients[old_p].m_cl.EnterReadLock();
			if (0 != g_clients[old_p].view_list.count(user_id)) {
				g_clients[old_p].m_cl.LeaveReadLock();
				send_leave_packet(old_p, user_id);
			}
			else {
				g_clients[old_p].m_cl.LeaveReadLock();
			}
		}
	}
}

void random_move_npc(int id)
{
	int x = g_clients[id].x;
	int y = g_clients[id].y;
	switch (rand() % 4) {
	case 0:	if (x < (WORLD_WIDTH - 1))	x++; break;
	case 1:	if (x > 0)	x--; break;
	case 2:	if (y < (WORLD_HEIGHT - 1))	y++; break;
	case 3:	if (y > 0)	y--; break;
	}

	if (g_clients[id].x / SECTOR_WIDTH != x / SECTOR_WIDTH || g_clients[id].y / SECTOR_WIDTH != y / SECTOR_WIDTH)
	{
		g_SectorLock[g_clients[id].y / SECTOR_WIDTH][g_clients[id].x / SECTOR_WIDTH].EnterWriteLock();
		g_ObjectListSector[g_clients[id].y / SECTOR_WIDTH][g_clients[id].x / SECTOR_WIDTH].erase(id);
		g_SectorLock[g_clients[id].y / SECTOR_WIDTH][g_clients[id].x / SECTOR_WIDTH].LeaveWriteLock();
		g_SectorLock[y / SECTOR_WIDTH][x / SECTOR_WIDTH].EnterWriteLock();
		g_ObjectListSector[y / SECTOR_WIDTH][x / SECTOR_WIDTH].insert(id);
		g_SectorLock[y / SECTOR_WIDTH][x / SECTOR_WIDTH].LeaveWriteLock();
	}

	g_clients[id].x = x;
	g_clients[id].y = y;

	for (int i = g_clients[id].y / SECTOR_WIDTH - 1; i <= g_clients[id].y / SECTOR_WIDTH + 1; ++i) {
		if (i < 0 || i > WORLD_HEIGHT / SECTOR_WIDTH - 1) continue;
		for (int j = g_clients[id].x / SECTOR_WIDTH - 1; j <= g_clients[id].x / SECTOR_WIDTH + 1; ++j) {
			if (j < 0 || j > WORLD_WIDTH / SECTOR_WIDTH - 1) continue;
			g_SectorLock[i][j].EnterReadLock();
			for (auto nearObj : g_ObjectListSector[i][j]) {
				if (ST_ACTIVE != g_clients[nearObj].m_status)	continue;
				if (false == is_player(nearObj))	continue;
				if (true == is_near(nearObj, id)) {
					g_clients[nearObj].m_cl.EnterReadLock();
					if (0 != g_clients[nearObj].view_list.count(id)) {
						g_clients[nearObj].m_cl.LeaveReadLock();
						send_move_packet(nearObj, id);
					}
					else {
						g_clients[nearObj].m_cl.LeaveReadLock();
						send_enter_packet(nearObj, id);
					}
				}
				else {
					g_clients[nearObj].m_cl.EnterReadLock();
					if (0 != g_clients[nearObj].view_list.count(id)) {
						g_clients[nearObj].m_cl.LeaveReadLock();
						send_leave_packet(nearObj, id);
					}
					else
						g_clients[nearObj].m_cl.LeaveReadLock();
				}
			}
			g_SectorLock[i][j].LeaveReadLock();
		}
	}
}

void enter_game(int user_id, char name[])
{
	g_clients[user_id].m_cl.EnterWriteLock();
	strcpy_s(g_clients[user_id].m_name, name);
	g_clients[user_id].m_name[MAX_ID_LEN] = NULL;
	send_login_ok_packet(user_id);
	g_clients[user_id].m_status = ST_ACTIVE;
	g_clients[user_id].m_cl.LeaveWriteLock();

	g_SectorLock[g_clients[user_id].y / SECTOR_WIDTH][g_clients[user_id].x / SECTOR_WIDTH].EnterWriteLock();
	g_ObjectListSector[g_clients[user_id].y / SECTOR_WIDTH][g_clients[user_id].x / SECTOR_WIDTH].insert(user_id);
	g_SectorLock[g_clients[user_id].y / SECTOR_WIDTH][g_clients[user_id].x / SECTOR_WIDTH].LeaveWriteLock();

	for (int i = g_clients[user_id].y / SECTOR_WIDTH - 1; i <= g_clients[user_id].y / SECTOR_WIDTH + 1; ++i) {
		if (i < 0 || i > WORLD_HEIGHT / SECTOR_WIDTH - 1) continue;
		for (int j = g_clients[user_id].x / SECTOR_WIDTH - 1; j <= g_clients[user_id].x / SECTOR_WIDTH + 1; ++j) {
			if (j < 0 || j > WORLD_WIDTH / SECTOR_WIDTH - 1) continue;
			g_SectorLock[i][j].EnterReadLock();
			for (auto nearObj : g_ObjectListSector[i][j]) {
				if (nearObj == user_id) continue;
				if (true == is_near(user_id, nearObj)) {
					if (ST_SLEEP == g_clients[nearObj].m_status) {
						activate_npc(nearObj);
					}
					if (ST_ACTIVE == g_clients[nearObj].m_status) {
						send_enter_packet(user_id, nearObj);
						if (true == is_player(nearObj))
							send_enter_packet(nearObj, user_id);
					}
				}
			}
			g_SectorLock[i][j].LeaveReadLock();
		}
	}
}

void process_packet(int user_id, char* buf)
{
	switch (buf[1]) {
	case C2S_LOGIN:
	{
		cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(buf);
		Point p = find_name_in_database(packet->name);
		bool canEnter = true;
		for (int i = 0; i < NPC_ID_START; ++i)
		{
			if (i == user_id)
				continue;
			if (g_clients[i].m_status == ST_ACTIVE)
			{
				string cmp1 = g_clients[i].m_name;
				string cmp2 = packet->name;
				if (cmp1 == cmp2)
				{
					canEnter = false;
					break;
				}
			}
		}
		if (p.x != -1 && canEnter)
		{
			g_clients[user_id].x = p.x;
			g_clients[user_id].y = p.y;
			enter_game(user_id, packet->name);
		}
		else
			disconnect(user_id);
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
		DebugBreak();
		exit(-1);
		break;
	}
}

// 멀티 스레드로 동작하기 전에 싱글 스레드로 돌아가는 함수라서 lock 할 필요가 없어요
// lock 걸면 오버헤드임
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
	set_pos_in_database(g_clients[user_id].m_name, g_clients[user_id].x, g_clients[user_id].y);

	g_SectorLock[g_clients[user_id].y / SECTOR_WIDTH][g_clients[user_id].x / SECTOR_WIDTH].EnterWriteLock();
	if (g_ObjectListSector[g_clients[user_id].y / SECTOR_WIDTH][g_clients[user_id].x / SECTOR_WIDTH].count(user_id) != 0)
		g_ObjectListSector[g_clients[user_id].y / SECTOR_WIDTH][g_clients[user_id].x / SECTOR_WIDTH].erase(user_id);
	g_SectorLock[g_clients[user_id].y / SECTOR_WIDTH][g_clients[user_id].x / SECTOR_WIDTH].LeaveWriteLock();

	send_leave_packet(user_id, user_id);

	g_clients[user_id].m_cl.EnterWriteLock();
	g_clients[user_id].m_status = ST_ALLOC;

	closesocket(g_clients[user_id].m_s);
	for (int i = 0; i < NPC_ID_START; ++i)
	{
		CLIENT& cl = g_clients[i];
		if (user_id == cl.m_id) continue;
		if (ST_ACTIVE == cl.m_status)
			send_leave_packet(cl.m_id, user_id);
	}
	g_clients[user_id].m_status = ST_FREE;
	g_clients[user_id].m_cl.LeaveWriteLock();
}

// 패킷 재조립
void recv_packet_construct(int user_id, int io_byte)
{
	CLIENT& cu = g_clients[user_id];
	EXOVER& r_o = cu.m_recv_over;

	// 나머지 바이트
	int rest_byte = io_byte;
	// 처리할 데이터의 포인터
	char* p = r_o.io_buf;
	int packet_size = 0;
	// 미리 받아논 게 있다면?
	if (0 != cu.m_prev_size)	packet_size = cu.m_packet_buf[0];
	// 처리할 데이터가 남아있다면?
	while (rest_byte > 0)
	{
		// 우리가 처리해야 되는 패킷을 전에 처리해본 적이 없을 때
		// == 패킷의 시작 부분이 있다
		if (0 == packet_size)	packet_size = *p;
		// 패킷을 완성할 수 있다
		if (packet_size <= rest_byte + cu.m_prev_size) {
			memcpy(cu.m_packet_buf + cu.m_prev_size, p, packet_size - cu.m_prev_size);
			p += packet_size - cu.m_prev_size;
			rest_byte -= packet_size - cu.m_prev_size;
			packet_size = 0;	// 이 패킷은 처리되었다
			process_packet(user_id, cu.m_packet_buf);
			cu.m_prev_size = 0;
		}
		else // 완성할 수 없다
		{
			// 저장하여 다음 번에 재조립 해줘야 함
			memcpy(cu.m_packet_buf + cu.m_prev_size, p, rest_byte);
			cu.m_prev_size += rest_byte;
			rest_byte = 0;
			p += rest_byte;
		}
	}
}

void worker_thread()
{
	while (true) {

		DWORD io_byte;
		ULONG_PTR key;
		WSAOVERLAPPED* over;
		GetQueuedCompletionStatus(g_iocp, &io_byte, &key, &over, INFINITE);

		EXOVER* exover = reinterpret_cast<EXOVER*>(over);
		int user_id = static_cast<int>(key);
		CLIENT& cl = g_clients[user_id];

		switch (exover->op) {
		case OP_RECV:
			// send나 recv의 경우에만 이 처리를 해줘야 함
			if (0 == io_byte)
				disconnect(user_id);
			else {
				recv_packet_construct(user_id, io_byte);
				ZeroMemory(&cl.m_recv_over.over, sizeof(cl.m_recv_over.over));
				DWORD flags = 0;
				WSARecv(cl.m_s, &cl.m_recv_over.wsabuf, 1, NULL, &flags, &cl.m_recv_over.over, NULL);
			}
			break;
		case OP_SEND:
			// send나 recv의 경우에만 이 처리를 해줘야 함
			if (0 == io_byte)
				disconnect(user_id);
			delete exover;
			break;
		case OP_ACCEPT:
		{
			// 메인에서 비동기 ACCEPT를 하고, 워커 쓰레드에서 완료하고,
			// 완료한 쓰레드에서 ACEEPT를 할 때까지 이 코드는 싱글 스레드로 돌아간다.
			// 그래서 락을 걸 필요가 없다.
			// 그러나 동시에 여러 클라이언트가 접속하는 경우(서버가 다운되서 다시 오픈하는 경우)
			// 그런 경우에는 여러 쓰레드에서 동시에 ACCEPT가 될 수 있기 때문에 락을 걸어야 한다.
			int user_id = -1;
			for (int i = 0; i < MAX_USER; ++i) {
				// lock_guard가 등록된 블록을 빠져나갈 때 unlock을 진행
				// 루프를 돌때마다 락이 등록되고, 사라지는 것을 반복하게 됨
				// break 뿐만 아니라 return, continue 등에도 unlock이 적용
				if (ST_FREE == g_clients[i].m_status) {
					g_clients[i].m_status = ST_ALLOC;
					user_id = i;
					break;
				}
			}

			SOCKET c_socket = exover->c_socket;
			if (-1 == user_id)
				closesocket(c_socket);
			else {
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket), g_iocp, user_id, 0);

				CLIENT& nc = g_clients[user_id];
				// 안쓰이는 거 골라내는 것도 해야하지만 나중에 생각
				nc.m_prev_size = 0;
				nc.m_recv_over.op = OP_RECV;
				ZeroMemory(&nc.m_recv_over.over, sizeof(nc.m_recv_over.over));
				nc.m_recv_over.wsabuf.buf = nc.m_recv_over.io_buf;
				nc.m_recv_over.wsabuf.len = MAX_BUF_SIZE;
				nc.m_s = c_socket;
				nc.view_list.clear();
				nc.x = rand() % WORLD_WIDTH;
				nc.y = rand() % WORLD_HEIGHT;
				DWORD flags = 0;
				WSARecv(c_socket, &nc.m_recv_over.wsabuf, 1, NULL, &flags, &nc.m_recv_over.over, NULL);
			}
			// Accept 새로 만드는 과정
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
			for (int i = 0; i < NPC_ID_START; ++i) {
				if (true == is_near(user_id, i))
					if (ST_ACTIVE == g_clients[i].m_status) {
						keep_alive = true;
						break;
					}
			}
			if (true == keep_alive) add_timer(user_id, OP_RANDOM_MOVE, 1000, 0);
			else g_clients[user_id].m_status = ST_SLEEP;
			delete exover;
		}
		break;
		case OP_PLAYER_MOVE:
		{
			g_clients[user_id].lua_l.EnterWriteLock();
			lua_State* L = g_clients[user_id].L;
			lua_getglobal(L, "event_player_move");
			lua_pushnumber(L, exover->p_id);	
			lua_pcall(L, 1, 0, 0);
			//lua_pop(L, 1);
			g_clients[user_id].lua_l.LeaveWriteLock();
			delete exover;
		}
		break;
		case OP_RUN:
		{
			g_clients[user_id].lua_l.EnterWriteLock();
			lua_State* L = g_clients[user_id].L;
			lua_getglobal(L, "event_run");
			lua_pushnumber(L, exover->p_id);
			lua_pcall(L, 1, 0, 0);
			//lua_pop(L, 1);
			g_clients[user_id].lua_l.LeaveWriteLock();
			delete exover;
		}
		break;
		case OP_RUN_FINISH:
		{
			g_clients[user_id].lua_l.EnterWriteLock();
			lua_State* L = g_clients[user_id].L;
			lua_getglobal(L, "event_run_finished");
			lua_pushnumber(L, exover->p_id);
			lua_pcall(L, 1, 0, 0);
			g_clients[user_id].lua_l.LeaveWriteLock();
			delete exover;
		}
		break;
		default:
			cout << "Unknown Operation in worker_thread!!\n";
			while (true);
		}
	}
}

int API_SendMessage(lua_State* L)
{
	int my_id = (int)lua_tointeger(L, -3);
	int user_id = (int)lua_tointeger(L, -2);
	char* mess = (char*)lua_tostring(L, -1);

	send_chat_packet(user_id, my_id, mess);
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

int API_add_timer_run(lua_State* L)
{
	int my_id = (int)lua_tointeger(L, -2);
	int user_id = (int)lua_tointeger(L, -1);
	add_timer(my_id, OP_RUN, 1000, user_id);
	lua_pop(L, 2);
	return 0;
}

int API_run_finished(lua_State* L)
{
	int my_id = (int)lua_tointeger(L, -2);
	int user_id = (int)lua_tointeger(L, -1);

	EXOVER* over = new EXOVER;
	over->op = OP_RUN_FINISH;
	over->p_id = user_id;

	PostQueuedCompletionStatus(g_iocp, 1, my_id, &over->over);
	lua_pop(L, 2);
	return 0;
}

void init_npc()
{
	for (int i = NPC_ID_START; i < NPC_ID_START + NUM_NPC; ++i) {
		g_clients[i].m_s = 0;
		g_clients[i].m_id = i;
		sprintf_s(g_clients[i].m_name, "NPC%d", i);
		g_clients[i].m_status = ST_SLEEP;
		g_clients[i].x = rand() % WORLD_WIDTH;
		g_clients[i].y = rand() % WORLD_HEIGHT;

		g_SectorLock[g_clients[i].y / SECTOR_WIDTH][g_clients[i].x / SECTOR_WIDTH].EnterWriteLock();
		g_ObjectListSector[g_clients[i].y / SECTOR_WIDTH][g_clients[i].x / SECTOR_WIDTH].insert(g_clients[i].m_id);
		g_SectorLock[g_clients[i].y / SECTOR_WIDTH][g_clients[i].x / SECTOR_WIDTH].LeaveWriteLock();

		//g_clients[i].m_last_move_time = high_resolution_clock::now();
		//add_timer(i, OP_RANDOM_MOVE, 1000);
		lua_State* L = g_clients[i].L = luaL_newstate();
		luaL_openlibs(L);
		luaL_loadfile(L, "NPC.LUA");
		int error = lua_pcall(L, 0, 0, 0);
		if (error) cout << lua_tostring(L, -1);
		lua_getglobal(L, "set_uid");
		lua_pushnumber(L, i);
		error = lua_pcall(L, 1, 0, 0);
		if (error) cout << lua_tostring(L, -1);
		//lua_pop(L, 1);

		lua_register(L, "API_send_message", API_SendMessage);
		lua_register(L, "API_get_x", API_get_x);
		lua_register(L, "API_get_y", API_get_y);
		lua_register(L, "API_add_timer_run", API_add_timer_run);
		lua_register(L, "API_run_finished", API_run_finished);
	}
}

void do_timer()
{
	while (true) {
		this_thread::sleep_for(1ms);
		while (true) {
			timer_lock.EnterWriteLock();
			if (true == timer_queue.empty()) {
				timer_lock.LeaveWriteLock();
				break;
			}
			auto now_t = high_resolution_clock::now();
			event_type temp_ev = timer_queue.top();
			if (timer_queue.top().wakeup_time > high_resolution_clock::now()) {
				timer_lock.LeaveWriteLock();
				break;
			}
			event_type ev = timer_queue.top();
			timer_queue.pop();
			timer_lock.LeaveWriteLock();
			switch (ev.event_id) {
			case OP_RANDOM_MOVE:
			case OP_RUN:
				EXOVER* over = new EXOVER;
				over->op = ev.event_id;
				over->p_id = ev.target_id;
				PostQueuedCompletionStatus(g_iocp, 1, ev.obj_id, &over->over);
				//random_move_npc(ev.obj_id);
				//add_timer(ev.obj_id, ev.event_id, 1000);
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
	cout << "NPC Initialize finished.\n";

	l_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN s_address;
	memset(&s_address, 0, sizeof(s_address));
	s_address.sin_family = AF_INET;
	s_address.sin_port = htons(SERVER_PORT);
	s_address.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	::bind(l_socket, reinterpret_cast<sockaddr*>(&s_address), sizeof(s_address));

	listen(l_socket, SOMAXCONN);

	g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	initialize_clients();

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(l_socket), g_iocp, 999, 0);
	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	EXOVER accept_over;
	ZeroMemory(&accept_over.over, sizeof(accept_over.over));
	accept_over.op = OP_ACCEPT;
	accept_over.c_socket = c_socket;
	AcceptEx(l_socket, c_socket, accept_over.io_buf, NULL, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &accept_over.over);	// 클라이언트 접속에 사용할 소켓을 미리 만들어놔야 함

	vector <thread> worker_threads;
	for (int i = 0; i < 4; ++i) worker_threads.emplace_back(worker_thread);

	//thread ai_thread{ do_ai };
	//ai_thread.join();
	thread timer_thread{ do_timer };
	timer_thread.join();
	for (auto& th : worker_threads)th.join();
}