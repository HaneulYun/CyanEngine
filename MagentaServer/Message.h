#pragma once

struct Message
{
	char msgId;
	int lParam;
	int mParam;
	int rParam;
};

// 임시 메시지
// Client to Server
#define ADD 0x0041
#define SUB 0x0042
// Server to Clients
#define EQUAL 0x0045

//////////////////////////////////////////////
// Client to Server
#define MESSAGE_READY 0x1000	// lParam = 보내는 클라의 아이디


//////////////////////////////////////////////
// Server to Clients
#define MESSAGE_YOUR_ID 0x0000	// lParam = 클라의 아이디
#define MESSAGE_CONNECTED_IDS 0x0001 // 서버와 연결된 클라이언트들 ID 반환.
#define MESSAGE_GAME_START 0x0002