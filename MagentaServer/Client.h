#pragma once
#include <atomic>
#include <chrono>
#include "RWLock.h"
#include "etc.h"
#include "protocol.h"

using namespace std;
using namespace chrono;

struct Inform {
	short	x, y;
	short hp;
	short level;
	int	exp;

	char	m_name[MAX_ID_LEN + 1];
};

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
	char	m_otype;
	Inform	m_inform;

	unsigned m_move_time;
	high_resolution_clock::time_point m_last_move_time;

	unordered_set<int> view_list;

	lua_State* L;
	RWLock lua_l;

	void heal_player()
	{
		int maxHp = 98 + pow(2, m_inform.level);
		if (m_inform.hp >= maxHp)
			return;
		m_inform.hp += maxHp * 0.1;
		if (m_inform.hp >= maxHp)
			m_inform.hp = maxHp;
	}

	void exp_plus(int exp)
	{
		int toNextLevelExp = 100 * pow(2, m_inform.level - 1);
		
		m_inform.exp += exp;
		if (m_inform.exp >= toNextLevelExp)
		{
			m_inform.exp -= toNextLevelExp;
			m_inform.level++;
		}
	}
};