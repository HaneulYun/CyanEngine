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

// 임시 메시지
// Client to Server
#define ADD 0x0041
#define SUB 0x0042
// Server to Clients
#define EQUAL 0x0045

//////////////////////////////////////////////
// Client to Server
#define MESSAGE_READY 0x0009	// 일단 보류


//////////////////////////////////////////////
// Server to Clients
#define MESSAGE_YOUR_ID 0x0000	// 클라이언트의 아이디 반환
#define MESSAGE_CONNECTED_IDS 0x0001 // 서버와 연결된 클라이언트들 ID 반환.