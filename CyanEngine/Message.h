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
#define MESSAGE_READY 0x90	// lParam = ������ Ŭ���� ���̵�

// lParam = ObjectID / mParam = �� ��ȣ�� ID / rParam = ����(radian)
#define MESSAGE_REQUEST_BULLET_CREATION 0x80	
#define MESSAGE_REQUEST_BULLET_CREATION_STRAIGHT 0x81
#define MESSAGE_REQUEST_BULLET_CREATION_CANNON 0x82
#define MESSAGE_REQUEST_BULLET_CREATION_SHARP 0x83
#define MESSAGE_REQUEST_BULLET_CREATION_LASER 0x84
#define MESSAGE_REQUEST_BULLET_CREATION_GUIDED 0x85


//////////////////////////////////////////////
// Server to Clients
#define MESSAGE_YOUR_ID 0x00	// lParam = Ŭ���� ���̵�
#define MESSAGE_CONNECTED_IDS 0x01 // ������ ����� Ŭ���̾�Ʈ�� ID ��ȯ.
#define MESSAGE_GAME_START 0x02	

// lParam = ObjectID / mParam = �� ��ȣ�� ID / rParam = ����(radian)
#define MESSAGE_CREATE_BULLET 0x10

// lParam = ObjectID / mParam = None / rParam = ����(radian)
//#define MESSAGE_CREATE_ENEMY 0x20
#define MESSAGE_CREATE_ENEMY_COMINGRECT 0x20
#define MESSAGE_CREATE_ENEMY_ROTATINGRECT 0x21
#define MESSAGE_CREATE_ENEMY_COMINGBIGRECT 0x22
#define MESSAGE_CREATE_ENEMY_COMINGTWOTRIANGLE 0x23
#define MESSAGE_CREATE_ENEMY_WHIRLINGCOMINGRECT 0x24
