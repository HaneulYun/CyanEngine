#include <iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma comment (lib, "WS2_32.lib")
#pragma comment(lib, "mswsock.lib")

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <unordered_set>
#include <atomic>
#include <chrono>
using namespace std;
using namespace chrono;

#include "protocol.h"
constexpr auto MAX_PACKET_SIZE = 255;
constexpr auto MAX_BUF_SIZE = 1024;
constexpr auto MAX_USER = 20000;
constexpr auto MAX_NPC = 200000;

constexpr auto VIEW_RADIUS = 8;

enum ENUMOP { OP_RECV, OP_SEND, OP_ACCEPT, MOVE_EVENT };
enum C_STATUS { ST_FREE, ST_ALLOC, ST_ACTIVE };

struct EXOVER
{
	WSAOVERLAPPED	over; // 맨 위에 overlapped 구조체가 있어야 한다.
	ENUMOP			op;
	char			io_buf[MAX_BUF_SIZE];
	union {
		WSABUF		wsabuf;
		SOCKET		c_socket;
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

	unordered_set<int> view_list;
	unordered_set<int> view_list_npc;
};
CLIENT g_clients[MAX_USER];

struct NPC
{
	atomic_bool m_is_active;
	EXOVER exover;
	int m_id;
	short x, y;
};
NPC g_npcs[MAX_NPC];

struct SECTOR
{
	unordered_set<int> ids;
	mutex sl;

	void fill_list(unordered_set<int>& set, int x, int y)
	{
		sl.lock();
		for (int id : ids)
		{
			if (id < MAX_USER)
			{
				if (abs(x - g_clients[id].x) > VIEW_RADIUS) continue;
				if (abs(y - g_clients[id].y) > VIEW_RADIUS) continue;
			}
			else
			{
				if (abs(x - g_npcs[id - MAX_USER].x) > VIEW_RADIUS) continue;
				if (abs(y - g_npcs[id - MAX_USER].y) > VIEW_RADIUS) continue;
			}
			set.insert(id);
		}
		sl.unlock();
	}
};

constexpr auto SECTOR_WIDTH = 10;
constexpr auto SECTOR_HEIGHT = 10;
constexpr auto SECTOR_WIDTH_COUNT = WORLD_WIDTH / SECTOR_WIDTH;
constexpr auto SECTOR_HEIGHT_COUNT = WORLD_HEIGHT / SECTOR_HEIGHT;;
struct SPM
{
	SECTOR sectors[SECTOR_HEIGHT_COUNT][SECTOR_WIDTH_COUNT];

	void insert(int id, int x, int y)
	{
		auto& sector = sectors[y / SECTOR_HEIGHT][x / SECTOR_WIDTH];
		sector.sl.lock();
		sector.ids.insert(id);
		sector.sl.unlock();
	}
	void erase(int id, int x, int y)
	{
		auto& sector = sectors[y / SECTOR_HEIGHT][x / SECTOR_WIDTH];
		if (sector.ids.count(id))
		{
			sector.sl.lock();
			sector.ids.erase(id);
			sector.sl.unlock();
		}
	}
	void update(int id, int ox, int oy, int nx, int ny)
	{
		auto& os = sectors[oy / SECTOR_HEIGHT][ox / SECTOR_WIDTH];
		auto& ns = sectors[ny / SECTOR_HEIGHT][nx / SECTOR_WIDTH];
		if (&os == &ns) return;
		erase(id, ox, oy);
		insert(id, nx, ny);
	}

	void fill_list(unordered_set<int>& set, int x, int y)
	{
		int l = (x - VIEW_RADIUS) / SECTOR_WIDTH;
		int r = (x + VIEW_RADIUS) / SECTOR_WIDTH;
		int b = (y - VIEW_RADIUS) / SECTOR_HEIGHT;
		int t = (y + VIEW_RADIUS) / SECTOR_HEIGHT;
		if (l < 0) l = 0; if (r > SECTOR_WIDTH_COUNT - 1) r = SECTOR_WIDTH_COUNT - 1;
		if (b < 0) b = 0; if (t > SECTOR_HEIGHT_COUNT - 1) t = SECTOR_HEIGHT_COUNT - 1;
		for (int j = b; j <= t; ++j)
			for (int i = l; i <= r; ++i)
				sectors[j][i].fill_list(set, x, y);
	}
};
SPM spm;

struct event_type
{
	int obj_id;
	high_resolution_clock::time_point wakeup_time;
	int event_id;
	int target_id;

	constexpr bool operator<(const event_type& rhs) const
	{
		return wakeup_time > rhs.wakeup_time;
	}
};
priority_queue<event_type> timer_queue;
mutex timer_lock;

bool CAS(atomic_bool* x, bool c, bool n)
{
	return atomic_compare_exchange_strong(x, &c, n);
}

void add_timer(int obj_id, int event_id, int wakeup_time)
{
	event_type e{};
	e.obj_id = obj_id;
	e.wakeup_time = high_resolution_clock::now() + seconds(wakeup_time / 1000);
	e.event_id = event_id;
	timer_lock.lock();
	timer_queue.push(e);
	timer_lock.unlock();
}

HANDLE g_iocp;
SOCKET l_socket;

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
	exover->wsabuf.len = buf[0];
	memcpy(exover->io_buf, buf, buf[0]);

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


	if (o_id < MAX_USER)
	{
		p.x = g_clients[o_id].x;
		p.y = g_clients[o_id].y;
		strcpy_s(p.name, g_clients[o_id].m_name);
		p.o_type = O_PLAYER;

		g_clients[user_id].m_cl.lock();
		g_clients[user_id].view_list.insert(o_id);
		g_clients[user_id].m_cl.unlock();
	}
	else
	{
		p.x = g_npcs[o_id - MAX_USER].x;
		p.y = g_npcs[o_id - MAX_USER].y;
		strcpy_s(p.name, "");
		p.o_type = O_NPC;

		g_clients[user_id].m_cl.lock();
		g_clients[user_id].view_list_npc.insert(o_id);
		g_clients[user_id].m_cl.unlock();
	}
	send_packet(user_id, &p);
}

void send_leave_packet(int user_id, int o_id)
{
	sc_packet_leave p;
	p.id = o_id;
	p.size = sizeof(p);
	p.type = S2C_LEAVE;

	g_clients[user_id].m_cl.lock();
	if(o_id < MAX_USER)
		g_clients[user_id].view_list.erase(o_id);
	else
		g_clients[user_id].view_list_npc.erase(o_id);
	g_clients[user_id].m_cl.unlock();

	send_packet(user_id, &p);
}

void send_move_packet(int user_id, int mover)
{
	sc_packet_move p;
	p.id = mover;
	p.size = sizeof(p);
	p.type = S2C_MOVE;
	if (mover < MAX_USER)
	{
		p.x = g_clients[mover].x;
		p.y = g_clients[mover].y;
		p.move_time = g_clients[mover].m_move_time;
	}
	else
	{
		p.x = g_npcs[mover - MAX_USER].x;
		p.y = g_npcs[mover - MAX_USER].y;
		p.move_time = 0;
	}

	send_packet(user_id, &p);
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
	spm.update(u.m_id, u.x, u.y, x, y);
	u.x = x;
	u.y = y;

	g_clients[user_id].m_cl.lock();
	unordered_set<int> old_vl = g_clients[user_id].view_list;
	g_clients[user_id].m_cl.unlock();

	unordered_set<int> new_vl;
	spm.fill_list(new_vl, g_clients[user_id].x, g_clients[user_id].y);

	send_move_packet(user_id, user_id);

	if (new_vl.count(user_id))
		new_vl.erase(user_id);
	for (auto id : new_vl) {
		if (id < MAX_USER)
		{
			if (0 == old_vl.count(id)) {
				send_enter_packet(user_id, id);
				g_clients[id].m_cl.lock();
				if (0 == g_clients[id].view_list.count(user_id))
				{
					g_clients[id].m_cl.unlock();
					send_enter_packet(id, user_id);
				}
				else
				{
					g_clients[id].m_cl.unlock();
					send_move_packet(id, user_id);
				}
			}
			else
			{
				g_clients[id].m_cl.lock();
				if (0 != g_clients[id].view_list.count(user_id))
				{
					g_clients[id].m_cl.unlock();
					send_move_packet(id, user_id);
				}
				else
				{
					g_clients[id].m_cl.unlock();
					send_enter_packet(id, user_id);
				}
			}
		}
		else
		{
			if (g_clients[user_id].m_move_time) continue;
			if (id < MAX_USER) continue;
			if (g_npcs[id - MAX_USER].m_is_active) continue;
			if (CAS(&g_npcs[id - MAX_USER].m_is_active, false, true))
				add_timer(g_npcs[id - MAX_USER].m_id, MOVE_EVENT, 1000);
		}
	}

	for (auto old_p : old_vl)
	{
		if (0 == new_vl.count(old_p))
		{
			send_leave_packet(user_id, old_p);
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
}

void enter_game(int user_id, char name[])
{
	g_clients[user_id].m_cl.lock();
	strcpy(g_clients[user_id].m_name, name);
	g_clients[user_id].m_name[MAX_ID_LEN] = NULL;
	send_login_ok_packet(user_id);
	g_clients[user_id].m_status = ST_ACTIVE;
	g_clients[user_id].m_cl.unlock();

	for (int i = 0; i < MAX_USER; i++)
	{
		if (user_id == i) continue;
		if (true == is_near(user_id, i))
		{
			//g_clients[i].m_cl.lock();
			if (ST_ACTIVE == g_clients[i].m_status)
				if (user_id != i)
				{
					send_enter_packet(user_id, i);
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
	spm.erase(user_id, g_clients[user_id].x, g_clients[user_id].y);
	closesocket(g_clients[user_id].m_s);
	for (auto& cl : g_clients)
	{
		if (user_id == cl.m_id) continue;
		//cl.m_cl.lock();
		if (ST_ACTIVE == cl.m_status)
			send_leave_packet(cl.m_id, user_id);
		//cl.m_cl.unlock();
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

void broadcast_move(int id)
{
	NPC& npc = g_npcs[id - MAX_USER];

	unordered_set<int> new_vl;
	spm.fill_list(new_vl, npc.x, npc.y);

	for (auto np : new_vl)
	{
		if (np >= MAX_USER) continue;

		auto& cl = g_clients[np];
		cl.m_cl.lock();
		int count = cl.view_list_npc.count(id);
		cl.m_cl.unlock();
		if (count == 0)
			send_enter_packet(cl.m_id, id);
		else
			send_move_packet(cl.m_id, id);
	}
}

void move_npc(int id)
{
	NPC& npc = g_npcs[id - MAX_USER];
	int x = npc.x;
	int y = npc.y;
	switch (rand() % 4)
	{
	case D_UP: if (y < (WORLD_HEIGHT - 1)) ++y;
		break;
	case D_DOWN: if (y > 0) --y;
		break;
	case D_LEFT: if (x > 0) --x;
		break;
	case D_RIGHT: if (x < (WORLD_WIDTH - 1)) ++x;
		break;
	default:
		cout << "Unknown Direction from Client move packet!\n";
		DebugBreak();
		exit(-1);
	}
	spm.update(npc.m_id, npc.x, npc.y, x, y);
	npc.x = x;
	npc.y = y;

	broadcast_move(id);
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

				spm.insert(nc.m_id, nc.x, nc.y);

				DWORD flags = 0;
				WSARecv(c_socket, &nc.m_recv_over.wsabuf, 1, NULL, &flags, &nc.m_recv_over.over, NULL);
			}

			c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			exover->c_socket = c_socket;
			ZeroMemory(&exover->over, sizeof(exover->over));
			AcceptEx(l_socket, c_socket, exover->io_buf, NULL, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &exover->over);
		}
		break;
		case MOVE_EVENT:
		{
			move_npc(user_id);

			unordered_set<int> new_vl;
			spm.fill_list(new_vl, g_npcs[user_id - MAX_USER].x, g_npcs[user_id - MAX_USER].y);

			bool near_player_exist = false;

			for (auto id : new_vl)
				if (id < MAX_USER)
				{
					if (g_clients[id].m_move_time) continue;
					near_player_exist = true;
					break;
				}

			if (near_player_exist)
				add_timer(user_id, MOVE_EVENT, 1000);
			else
			{
				g_npcs[user_id - MAX_USER].m_is_active = false;

				for (auto& cl : g_clients)
				{
					if (cl.m_status != ST_ACTIVE) continue;
					send_leave_packet(cl.m_id, user_id);
				}
			}
		}
		break;
		}
	}
}

void process_event(const event_type& e)
{
	EXOVER* overlap_ex = &g_npcs[e.obj_id - MAX_USER].exover;
	ZeroMemory(&overlap_ex->over, sizeof(overlap_ex->over));
	overlap_ex->op = MOVE_EVENT;
	PostQueuedCompletionStatus(g_iocp, 1, e.obj_id, &overlap_ex->over);
}


void NPC_Create()
{
	high_resolution_clock::time_point currentTime = high_resolution_clock::now();
	for (int i = 0; i < MAX_NPC; ++i)
	{
		g_npcs[i].m_is_active = false;
		g_npcs[i].m_id = MAX_USER + i;
		g_npcs[i].x = rand() % WORLD_WIDTH;
		g_npcs[i].y = rand() % WORLD_HEIGHT;
		spm.insert(g_npcs[i].m_id, g_npcs[i].x, g_npcs[i].y);
	}
}

void timer_thread()
{
	NPC_Create();

	do {
		Sleep(1);
		do {
			if (!timer_queue.size())
				break;
			high_resolution_clock::time_point current_time = high_resolution_clock::now();
			timer_lock.lock();
			event_type e = timer_queue.top();
			if (e.wakeup_time > current_time)
			{
				timer_lock.unlock();
				break;
			}
			timer_queue.pop();
			timer_lock.unlock();
			process_event(e);
		} while (true);
	} while (true);
}

int main()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

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
	thread timer_thread{ ::timer_thread };
	for (int i = 0; i < 4; ++i) worker_threads.emplace_back(worker_thread);

	timer_thread.join();
	for (auto& th : worker_threads) th.join();
}