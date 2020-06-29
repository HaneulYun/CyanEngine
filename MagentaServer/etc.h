#pragma once
#include <chrono>

using namespace std;
using namespace chrono;

enum C_STATUS { ST_FREE, ST_ALLOC, ST_ACTIVE, ST_SLEEP, ST_DEAD };
enum ENUMOP { OP_RECV, OP_SEND, OP_ACCEPT, OP_RANDOM_MOVE, OP_PATHFIND, OP_PLAYER_MOVE, OP_RUN, OP_RUN_FINISH, OP_HEAL, OP_RESPAWN };

constexpr auto MAX_PACKET_SIZE = 255;
constexpr auto MAX_BUF_SIZE = 1024;
constexpr auto VIEW_RADIUS = 8;

struct event_type {
	int obj_id;
	ENUMOP event_id;
	high_resolution_clock::time_point wakeup_time;
	int target_id;
	int firstX;
	int firstY;

	constexpr bool operator <(const event_type& left) const
	{
		return (wakeup_time > left.wakeup_time);
	}
};

struct EXOVER {
	WSAOVERLAPPED	over;
	ENUMOP			op;
	char			io_buf[MAX_BUF_SIZE];
	union {
		WSABUF			wsabuf;
		SOCKET			c_socket;
		int				p_id;
	};
	int firstX;
	int firstY;
};

struct Point {
	short x1, y1;
	short x2, y2;

	friend istream& operator>>(istream& is, Point& p);
};

static istream& operator>>(istream& is, Point& p)
{
	is >> p.x1 >> p.y1 >> p.x2 >> p.y2;
	return is;
}