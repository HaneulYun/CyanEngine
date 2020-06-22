#pragma once
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

#include <unordered_map>
#include <unordered_set>
#include <Windows.h>
#include <string>
#include <iostream>
#include <thread>

#include "protocol.h"
#include "Client.h"
#include "Timer.h"
#include "Database.h"
#include "RWLock.h"
#include "etc.h"

#define SECTOR_WIDTH 20

using namespace std;

class IOCPServer {
private:
	HANDLE g_iocp;
	SOCKET l_socket;
	unordered_map<int, Client> g_clients;
	
	Timer timer;
	Database database;

	unordered_set<int> g_ObjectListSector[WORLD_HEIGHT / SECTOR_WIDTH][WORLD_WIDTH / SECTOR_WIDTH];
	RWLock g_SectorLock[WORLD_HEIGHT / SECTOR_WIDTH][WORLD_WIDTH / SECTOR_WIDTH];

	bool is_player(int id);
	bool is_near(int a, int b);

	void init_npc();
	void activate_npc(int id);
	void random_move_npc(int id);

	void initialize_clients();
	void process_packet(int user_id, char* buf);
	void recv_packet_construct(int user_id, int io_byte);
	void worker_thread();

	void enter_game(int user_id, char name[]);
	void do_move(int user_id, int direction);
	void disconnect(int user_id);

	void send_packet(int user_id, void* p);
	void send_login_ok_packet(int user_id);
	void send_login_fail_packet(int user_id);
	void send_enter_packet(int user_id, int o_id);
	void send_move_packet(int user_id, int mover);
	void send_leave_packet(int user_id, int o_id);
	void send_chat_packet(int user_id, int chatter, wchar_t mess[]);
	void send_stat_change_packet(int user_id);

	int API_SendMessage(lua_State* L);
	int API_get_x(lua_State* L);
	int API_get_y(lua_State* L);
	int API_add_timer_run(lua_State* L);
	int API_run_finished(lua_State* L);

public:
	IOCPServer();
	~IOCPServer();
};