#pragma once

constexpr int MAX_ID_LEN = 50;
constexpr int MAX_STR_LEN = 80;

#define WORLD_WIDTH		400
#define WORLD_HEIGHT	400

#define SERVER_PORT		9000
#define NPC_ID_START	20000
#define NUM_NPC			2000


#define C2S_LOGIN	1
#define C2S_MOVE	2

#define S2C_LOGIN_OK		1
#define S2C_MOVE			2
#define S2C_ENTER			3
#define S2C_LEAVE			4
#define S2C_CHAT			5

#pragma pack(push ,1)

struct sc_packet_login_ok {
	char size;
	char type;
	int id;
	short x, y;
	short hp;
	short level;
	int	exp;
};

struct sc_packet_move {
	char size;
	char type;
	int id;
	short x, y;
	unsigned move_time;
};

constexpr unsigned char D_UP = 0;
constexpr unsigned char D_DOWN = 1;
constexpr unsigned char D_LEFT = 2;
constexpr unsigned char D_RIGHT = 3;

struct sc_packet_enter {
	char size;
	char type;
	int id;
	char name[MAX_ID_LEN];
	char o_type;
	short x, y;
};

struct sc_packet_leave {
	char size;
	char type;
	int id;
};

struct sc_packet_chat {
	char size;
	char type;
	int	 id;
	char mess[MAX_STR_LEN];
};

struct cs_packet_login {
	char	size;
	char	type;
	char	name[MAX_ID_LEN];
};

constexpr unsigned char O_HUMAN= 0;
constexpr unsigned char O_ELF = 1;
constexpr unsigned char O_ORC = 2;

struct cs_packet_move {
	char	size;
	char	type;
	char	direction;
	unsigned move_time;
};

#pragma pack (pop)