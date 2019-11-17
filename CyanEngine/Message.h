#pragma once
#include <queue>
struct Message
{
	char msgId;
	int lParam;
	int mParam;
	int rParam;
};

static std::queue<Message> recvQueue;
static CRITICAL_SECTION rqcs;


//////////////////////////////////////////////
// Client to Server
#define MESSAGE_READY 0x1000	// lParam = 보내는 클라의 아이디


//////////////////////////////////////////////
// Server to Clients
#define MESSAGE_YOUR_ID 0x0000	// lParam = 클라의 아이디
#define MESSAGE_CONNECTED_IDS 0x0001 // 서버와 연결된 클라이언트들 ID 반환.
#define MESSAGE_GAME_START 0x0002