#include <iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "mswsock.lib")

#include <thread>
#include <mutex>
#include <unordered_set>
#include <atomic>
#include <queue>
using namespace std;
#include "protocol.h"

constexpr auto MAX_PACKET_SIZE = 255;
constexpr auto MAX_BUF_SIZE = 1024;
constexpr auto MAX_USER = 10000;
constexpr auto MAX_NPC = 200000;

constexpr auto VIEW_RADIUS = 8;
enum ENUMOP { OP_RECV, OP_SEND, OP_ACCEPT, OP_MOVE };
enum C_STATUS { ST_FREE, ST_ALLOC, ST_ACTIVE };

#define SECTOR_WIDTH 20

struct EXOVER {
	WSAOVERLAPPED	over;
	ENUMOP			op;
	char			io_buf[MAX_BUF_SIZE];
	union {
		WSABUF			wsabuf;
		SOCKET			c_socket;
	};
};

struct event_type {
	int		obj_id;
	chrono::high_resolution_clock::time_point wakeup_time;
	int		event_id;
	int		target_id;

	constexpr bool operator< (const event_type& _Left) const
	{
		return (wakeup_time > _Left.wakeup_time);
	}
};

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

unordered_set<int> g_ObjectListSector[WORLD_HEIGHT / SECTOR_WIDTH][WORLD_WIDTH / SECTOR_WIDTH];
RWLock g_SectorLock[WORLD_HEIGHT / SECTOR_WIDTH][WORLD_WIDTH / SECTOR_WIDTH];

struct CLIENT {
	RWLock	m_lock;
	SOCKET	m_s;
	int		m_id;	// 한번 세팅하면 바꾸지 않으니 mutex 할 필요 없다
	EXOVER	m_recv_over;
	int		m_prev_size;
	char	m_packet_buf[MAX_PACKET_SIZE];
	atomic <C_STATUS> m_status;

	short	x, y;
	char	m_name[MAX_ID_LEN + 1];
	unsigned m_move_time;

	unordered_set<int> view_list;
};

CLIENT g_clients[MAX_USER];
HANDLE g_iocp;
SOCKET l_socket;	// 한번 정해진 다음에 바뀌지 않으니 data race 발생하지 않음
EXOVER npcover;

void send_enter_npc_packet(int user_id, int o_id);
void send_leave_npc_packet(int user_id, int o_id);
void send_move_npc_packet(int user_id, int mover);

class NPC
{
public:
	RWLock m_lock;
	short x, y;
	int	m_id;
	atomic_bool is_activate;

	unordered_set<int> view_list;

	bool is_near(int a)
	{
		if (abs(g_clients[a].x - x) > VIEW_RADIUS)	return false;
		if (abs(g_clients[a].y - y) > VIEW_RADIUS)	return false;
		return true;
	}

	void move_npc()
	{
		int direction = rand() % 4;

		short prevX = x, prevY = y;
		switch (direction)
		{
		case D_UP:	if (y > 0)	y--;	break;
		case D_DOWN:if (y < (WORLD_HEIGHT - 1))	y++;	break;
		case D_LEFT:if (x > 0)	x--;	break;
		case D_RIGHT:if (x < (WORLD_WIDTH - 1)) x++;	break;
		default:
			break;
		}

		if (prevX / SECTOR_WIDTH != x / SECTOR_WIDTH || prevY / SECTOR_WIDTH != y / SECTOR_WIDTH)
		{
			g_SectorLock[prevY / SECTOR_WIDTH][prevX / SECTOR_WIDTH].EnterWriteLock();
			g_ObjectListSector[prevY / SECTOR_WIDTH][prevX / SECTOR_WIDTH].erase(m_id);
			g_SectorLock[prevY / SECTOR_WIDTH][prevX / SECTOR_WIDTH].LeaveWriteLock();
			g_SectorLock[y / SECTOR_WIDTH][x / SECTOR_WIDTH].EnterWriteLock();
			g_ObjectListSector[y / SECTOR_WIDTH][x / SECTOR_WIDTH].insert(m_id);
			g_SectorLock[y / SECTOR_WIDTH][x / SECTOR_WIDTH].LeaveWriteLock();
		}

		m_lock.EnterReadLock();
		unordered_set<int> old_vl = view_list;
		m_lock.LeaveReadLock();
		unordered_set<int> new_vl;
		for (int i = y / SECTOR_WIDTH - 1; i <= y / SECTOR_WIDTH + 1; ++i)
		{
			if (i < 0 || i > WORLD_HEIGHT / SECTOR_WIDTH - 1)
				continue;
			for (int j = x / SECTOR_WIDTH - 1; j <= x / SECTOR_WIDTH + 1; ++j)
			{
				if (j < 0 || j > WORLD_WIDTH / SECTOR_WIDTH - 1)
					continue;
				g_SectorLock[i][j].EnterReadLock();
				for (auto nearObj : g_ObjectListSector[i][j])
				{
					if (nearObj >= MAX_USER) continue;
					if (g_clients[nearObj].m_status != ST_ACTIVE) continue;
					if (true == is_near(nearObj))
					{
						new_vl.insert(nearObj);
					}
				}
				g_SectorLock[i][j].LeaveReadLock();
			}
		}

		for (auto np : new_vl)
		{
			if (0 == old_vl.count(np)) {
				send_enter_npc_packet(np, m_id);
			}
			else {
				send_move_npc_packet(np, m_id);
			}
		}

		for (auto old_p : old_vl) {
			if (0 == new_vl.count(old_p)) {
				send_leave_npc_packet(old_p, m_id);
			}
		}
	}
};

priority_queue<event_type> timer_queue;
mutex timer_lock;

NPC g_npcs[MAX_NPC];

void add_timer(int id, int ev_type, int time)
{
	timer_lock.lock();
	event_type t;
	t.event_id = OP_MOVE;
	t.target_id = id;
	t.wakeup_time = chrono::high_resolution_clock::now() +chrono::milliseconds(1000);
	timer_queue.push(t);
	timer_lock.unlock();
}

bool CAS(atomic_bool* x, bool c, bool n)
{
	return atomic_compare_exchange_strong(x, &c, n);
}

void NPC_Create()
{
	for (int i = 0; i < MAX_NPC; ++i) {
		g_npcs[i].x = rand() % WORLD_WIDTH;
		g_npcs[i].y = rand() % WORLD_HEIGHT;

		g_npcs[i].m_id = i + MAX_USER;

		g_SectorLock[g_npcs[i].y / SECTOR_WIDTH][g_npcs[i].x / SECTOR_WIDTH].EnterWriteLock();
		g_ObjectListSector[g_npcs[i].y / SECTOR_WIDTH][g_npcs[i].x / SECTOR_WIDTH].insert(g_npcs[i].m_id);
		g_SectorLock[g_npcs[i].y / SECTOR_WIDTH][g_npcs[i].x / SECTOR_WIDTH].LeaveWriteLock();

		g_npcs[i].is_activate = false;
		g_npcs[i].view_list.clear();
	}
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
	p.o_type = O_PLAYER;

	g_clients[user_id].m_lock.EnterWriteLock();
	g_clients[user_id].view_list.insert(o_id);
	g_clients[user_id].m_lock.LeaveWriteLock();

	send_packet(user_id, &p);
}

void send_enter_npc_packet(int user_id, int o_id)
{
	sc_packet_enter p;
	p.id = o_id;
	p.size = sizeof(p);
	p.type = S2C_ENTER;
	p.x = g_npcs[o_id - MAX_USER].x;
	p.y = g_npcs[o_id - MAX_USER].y;
	strcpy_s(p.name, "");// g_npcs[o_id].m_name);
	p.o_type = O_NPC;

	g_npcs[o_id - MAX_USER].m_lock.EnterWriteLock();
	g_npcs[o_id - MAX_USER].view_list.insert(user_id);
	g_npcs[o_id - MAX_USER].m_lock.LeaveWriteLock();

	send_packet(user_id, &p);
}

void send_leave_packet(int user_id, int o_id)
{
	sc_packet_leave p;
	p.id = o_id;
	p.size = sizeof(p);
	p.type = S2C_LEAVE;

	g_clients[user_id].m_lock.EnterWriteLock();
	g_clients[user_id].view_list.erase(o_id);
	g_clients[user_id].m_lock.LeaveWriteLock();

	send_packet(user_id, &p);
}

void send_leave_npc_packet(int user_id, int o_id)
{
	sc_packet_leave p;
	p.id = o_id;
	p.size = sizeof(p);
	p.type = S2C_LEAVE;

	g_npcs[o_id - MAX_USER].m_lock.EnterWriteLock();
	g_npcs[o_id - MAX_USER].view_list.erase(user_id);
	g_npcs[o_id - MAX_USER].m_lock.LeaveWriteLock();

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

void send_move_npc_packet(int user_id, int o_id)
{
	sc_packet_move p;
	p.id = o_id;
	p.size = sizeof(p);
	p.type = S2C_MOVE;
	p.x = g_npcs[o_id - MAX_USER].x;
	p.y = g_npcs[o_id - MAX_USER].y;

	send_packet(user_id, &p);
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

	//
	if (u.x / SECTOR_WIDTH != x / SECTOR_WIDTH || u.y / SECTOR_WIDTH != y / SECTOR_WIDTH)
	{
		g_SectorLock[u.y / SECTOR_WIDTH][u.x / SECTOR_WIDTH].EnterWriteLock();
		g_ObjectListSector[u.y / SECTOR_WIDTH][u.x / SECTOR_WIDTH].erase(user_id);
		g_SectorLock[u.y / SECTOR_WIDTH][u.x / SECTOR_WIDTH].LeaveWriteLock();
		g_SectorLock[y / SECTOR_WIDTH][x / SECTOR_WIDTH].EnterWriteLock();
		g_ObjectListSector[y / SECTOR_WIDTH][x / SECTOR_WIDTH].insert(user_id);
		g_SectorLock[y / SECTOR_WIDTH][x / SECTOR_WIDTH].LeaveWriteLock();
	}
	//

	u.x = x;
	u.y = y;

	//
	g_clients[user_id].m_lock.EnterReadLock();
	unordered_set<int> old_vl = g_clients[user_id].view_list;
	g_clients[user_id].m_lock.LeaveReadLock();
	unordered_set<int> new_vl;

	for (int i = u.y / SECTOR_WIDTH - 1; i <= u.y / SECTOR_WIDTH + 1; ++i)
	{
		if (i < 0 || i > WORLD_HEIGHT / SECTOR_WIDTH - 1)
			continue;
		for (int j = u.x / SECTOR_WIDTH - 1; j <= u.x / SECTOR_WIDTH + 1; ++j)
		{
			if (j < 0 || j > WORLD_WIDTH / SECTOR_WIDTH - 1)
				continue;
			g_SectorLock[i][j].EnterReadLock();
			for (auto nearObj : g_ObjectListSector[i][j])
			{
				if (nearObj == user_id) continue;
				if (nearObj < MAX_USER)
				{
					if (g_clients[nearObj].m_status != ST_ACTIVE)
						continue;
					if (true == is_near(nearObj, user_id))
					{
						new_vl.insert(nearObj);
					}
				}
				else
				{
					if (true == g_npcs[nearObj - MAX_USER].is_near(user_id))
					{
						if (0 == g_npcs[nearObj - MAX_USER].is_activate) {
							if (CAS(&g_npcs[nearObj - MAX_USER].is_activate, false, true))
								add_timer(nearObj - MAX_USER, OP_MOVE, 1000);
						}
					}
				}
			}
			g_SectorLock[i][j].LeaveReadLock();
		}
	}

	send_move_packet(user_id, user_id);
	//
	for (auto np : new_vl)
	{
		if (0 == old_vl.count(np)) {
			send_enter_packet(user_id, np);
			g_clients[np].m_lock.EnterReadLock();
			if (0 == g_clients[np].view_list.count(user_id)) {
				g_clients[np].m_lock.LeaveReadLock();
				send_enter_packet(np, user_id);
			}
			else {
				g_clients[np].m_lock.LeaveReadLock();
				send_move_packet(np, user_id);
			}
		}
		else {
			g_clients[np].m_lock.EnterReadLock();
			if (0 != g_clients[np].view_list.count(user_id)) {
				g_clients[np].m_lock.LeaveReadLock();
				send_move_packet(np, user_id);
			}
			else {
				g_clients[np].m_lock.LeaveReadLock();
				send_enter_packet(np, user_id);
			}
		}
	}

	for (auto old_p : old_vl){
		if (0 == new_vl.count(old_p)) {
			send_leave_packet(user_id, old_p);
			g_clients[old_p].m_lock.EnterReadLock();
			if (0 != g_clients[old_p].view_list.count(user_id)) {
				g_clients[old_p].m_lock.LeaveReadLock();
				send_leave_packet(old_p, user_id);
			}
			else {
				g_clients[old_p].m_lock.LeaveReadLock();
			}
		}
	}
	//
}

void enter_game(int user_id, char name[])
{
	g_SectorLock[g_clients[user_id].y / SECTOR_WIDTH][g_clients[user_id].x / SECTOR_WIDTH].EnterWriteLock();
	g_ObjectListSector[g_clients[user_id].y / SECTOR_WIDTH][g_clients[user_id].x / SECTOR_WIDTH].insert(user_id);
	g_SectorLock[g_clients[user_id].y / SECTOR_WIDTH][g_clients[user_id].x / SECTOR_WIDTH].LeaveWriteLock();
	
	unordered_set<int> new_vl;

	for (int i = g_clients[user_id].y / SECTOR_WIDTH - 1; i <= g_clients[user_id].y / SECTOR_WIDTH + 1; ++i)
	{
		if (i < 0 || i > WORLD_HEIGHT / SECTOR_WIDTH - 1)
			continue;
		for (int j = g_clients[user_id].x / SECTOR_WIDTH - 1; j <= g_clients[user_id].x / SECTOR_WIDTH + 1; ++j)
		{
			if (j < 0 || j > WORLD_WIDTH / SECTOR_WIDTH - 1)
				continue;
			g_SectorLock[i][j].EnterReadLock();
			for (auto nearObj : g_ObjectListSector[i][j])
			{
				if (nearObj >= MAX_USER)
				{
					if (true == g_npcs[nearObj - MAX_USER].is_near(user_id))
					{
						if (0 == g_npcs[nearObj - MAX_USER].is_activate) {
							if (CAS(&g_npcs[nearObj - MAX_USER].is_activate, false, true))
								add_timer(nearObj - MAX_USER, OP_MOVE, 1000);
						}
					}
				}
				else
				{
					if (nearObj == user_id || g_clients[nearObj].m_status != ST_ACTIVE) continue;
					if (true == is_near(nearObj, user_id))
					{
						new_vl.insert(nearObj);
					}
				}
			}
			g_SectorLock[i][j].LeaveReadLock();
		}
	}

	g_clients[user_id].m_lock.EnterWriteLock();
	strcpy_s(g_clients[user_id].m_name, name);
	g_clients[user_id].m_name[MAX_ID_LEN] = NULL;
	send_login_ok_packet(user_id);
	g_clients[user_id].m_status = ST_ACTIVE;
	g_clients[user_id].m_lock.LeaveWriteLock();

	for (auto nearObj : new_vl)
	{
		if (user_id == nearObj) continue;
		if (true == is_near(user_id, nearObj)) {
			if (ST_ACTIVE == g_clients[nearObj].m_status) {
				if (user_id != nearObj) {
					send_enter_packet(user_id, nearObj);
					send_enter_packet(nearObj, user_id);
				}
			}
		}
	}
}

void process_packet(int user_id, char* buf)
{
	switch (buf[1]) {
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
	//
	g_SectorLock[g_clients[user_id].y / SECTOR_WIDTH][g_clients[user_id].x / SECTOR_WIDTH].EnterWriteLock();
	if(g_ObjectListSector[g_clients[user_id].y / SECTOR_WIDTH][g_clients[user_id].x / SECTOR_WIDTH].count(user_id) != 0)
		g_ObjectListSector[g_clients[user_id].y / SECTOR_WIDTH][g_clients[user_id].x / SECTOR_WIDTH].erase(user_id);
	g_SectorLock[g_clients[user_id].y / SECTOR_WIDTH][g_clients[user_id].x / SECTOR_WIDTH].LeaveWriteLock();
	//

	send_leave_packet(user_id, user_id);

	g_clients[user_id].m_lock.EnterWriteLock();
	g_clients[user_id].m_status = ST_ALLOC;

	closesocket(g_clients[user_id].m_s);
	for (auto& cl : g_clients)
	{
		if (user_id == cl.m_id) continue;
		if (ST_ACTIVE == cl.m_status)
			send_leave_packet(cl.m_id, user_id);
	}
	g_clients[user_id].m_status = ST_FREE;
	g_clients[user_id].m_lock.LeaveWriteLock();
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
		case OP_MOVE:
		{
			g_npcs[user_id].move_npc();
			g_npcs[user_id].m_lock.EnterReadLock();
			if (!g_npcs[user_id].view_list.empty())	add_timer(user_id, OP_MOVE, 1000);
			else g_npcs[user_id].is_activate = false;
			g_npcs[user_id].m_lock.LeaveReadLock();
		}
			break;
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
		}
	}
}

void process_event(event_type ev)
{
	npcover.op = OP_MOVE;
	
	PostQueuedCompletionStatus(g_iocp, 0, ev.target_id, &npcover.over);
}

void timer_thread()
{
	do {
		Sleep(1);
		do {
			if (!timer_queue.empty()) {
				timer_lock.lock();
				event_type k = timer_queue.top();
				auto current_time = chrono::high_resolution_clock::now();
				if (k.wakeup_time > current_time)
				{
					timer_lock.unlock();
					break;
				}
				timer_queue.pop();
				timer_lock.unlock();
				process_event(k);
			}
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
	s_address.sin_port = htons(SERVER_PORT);
	s_address.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	::bind(l_socket, reinterpret_cast<sockaddr*>(&s_address), sizeof(s_address));

	listen(l_socket, SOMAXCONN);

	g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	NPC_Create();
	initialize_clients();

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(l_socket), g_iocp, 999, 0);
	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	EXOVER accept_over;
	ZeroMemory(&accept_over.over, sizeof(accept_over.over));
	accept_over.op = OP_ACCEPT;
	accept_over.c_socket = c_socket;
	AcceptEx(l_socket, c_socket, accept_over.io_buf, NULL, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &accept_over.over);	// 클라이언트 접속에 사용할 소켓을 미리 만들어놔야 함

	vector <thread> worker_threads;
	thread timer_threads(timer_thread);
	for (int i = 0; i < 1; ++i) worker_threads.emplace_back(worker_thread);
	for (auto& th : worker_threads)th.join();
	timer_threads.join();
}