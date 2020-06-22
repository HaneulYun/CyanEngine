#pragma once

enum C_STATUS { ST_FREE, ST_ALLOC, ST_ACTIVE, ST_SLEEP };
enum ENUMOP { OP_RECV, OP_SEND, OP_ACCEPT, OP_RANDOM_MOVE, OP_PLAYER_MOVE, OP_RUN, OP_RUN_FINISH };

constexpr auto MAX_PACKET_SIZE = 255;
constexpr auto MAX_BUF_SIZE = 1024;
constexpr auto VIEW_RADIUS = 8;

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