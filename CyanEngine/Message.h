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
#define MESSAGE_READY 0x40	// lParam = ������ Ŭ���� ���̵�


//////////////////////////////////////////////
// Server to Clients
#define MESSAGE_YOUR_ID 0x00	// lParam = Ŭ���� ���̵�
#define MESSAGE_CONNECTED_IDS 0x01 // ������ ����� Ŭ���̾�Ʈ�� ID ��ȯ.
#define MESSAGE_GAME_START 0x02