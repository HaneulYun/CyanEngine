#pragma once
#include <atomic>
#include <chrono>
#include "RWLock.h"
#include "etc.h"

using namespace std;
using namespace chrono;

class Client
{
public:
	RWLock	m_cl;
	SOCKET	m_s;
	int		m_id;	// 한번 세팅하면 바꾸지 않으니 mutex 할 필요 없다
	EXOVER	m_recv_over;
	int		m_prev_size;
	char	m_packet_buf[MAX_PACKET_SIZE];
	atomic <C_STATUS> m_status;

	short	x, y;
	short hp;
	short level;
	int	exp;

	char	m_name[MAX_ID_LEN + 1];
	unsigned m_move_time;
	high_resolution_clock::time_point m_last_move_time;

	unordered_set<int> view_list;
	lua_State* L;
	RWLock lua_l;
};