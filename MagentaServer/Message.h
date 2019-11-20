#pragma once

struct Message
{
	unsigned char msgId;
	int lParam;
	int mParam;
	int rParam;
};

// �ӽ� �޽���
// Client to Server
#define ADD 0x41
#define SUB 0x42
// Server to Clients
#define EQUAL 0x45

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
