#pragma once

constexpr int MAX_ID_LEN = 50;
constexpr int MAX_STR_LEN = 60;

#define WORLD_WIDTH		800
#define WORLD_HEIGHT	800

#define SERVER_PORT		9000
#define NPC_ID_START    20000
#define NUM_NPC			2000

#define C2S_LOGIN	1
#define C2S_MOVE	2
#define C2S_ATTACK  3
#define C2S_CHAT    4
#define C2S_LOGOUT  5

#define S2C_LOGIN_OK		1
#define S2C_LOGIN_FAIL		2
#define S2C_MOVE			3
#define S2C_ENTER			4
#define S2C_LEAVE			5
#define S2C_CHAT			6
#define S2C_STAT_CHANGE     7

#pragma pack(push ,1)

struct sc_packet_login_ok {
	unsigned char size;
	char type;
	int id;
	short x, y;
	short hp;
	short level;
	int	exp;
};

struct sc_packet_login_fail {
	unsigned char size;
	char type;
};

struct sc_packet_move {
	unsigned char size;
	char type;
	int id;
	short x, y;
	unsigned move_time;
};

constexpr unsigned char O_HUMAN = 0;
constexpr unsigned char O_ELF = 1;
constexpr unsigned char O_ORC = 2;

struct sc_packet_enter {
	unsigned char size;
	char type;
	int id;
	char name[MAX_ID_LEN];
	char o_type;
	short x, y;
};

struct sc_packet_leave {
	unsigned char size;
	char type;
	int id;
};

struct sc_packet_chat {
	unsigned char size;
	char type;
	int	 id;
	wchar_t mess[MAX_STR_LEN];
};

struct cs_packet_login {
	unsigned char	size;
	char	type;
	char	name[MAX_ID_LEN];
};

struct sc_packet_stat_change {
	unsigned char size;
	char type;
	short hp;
	short level;
	int	exp;
};

constexpr unsigned char D_UP = 0;
constexpr unsigned char D_DOWN = 1;
constexpr unsigned char D_LEFT = 2;
constexpr unsigned char D_RIGHT = 3;

struct cs_packet_move {
	unsigned char	size;
	char	type;
	char	direction;
	unsigned move_time;
};

struct cs_packet_attack {
	unsigned char size;
	char type;
};

struct cs_packet_chat {
	unsigned char size;
	char type;
	wchar_t message[MAX_STR_LEN];
};

struct cs_packet_logout {
	unsigned char size;
	char type;
};
#pragma pack (pop)