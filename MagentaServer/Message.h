#pragma once

struct Message
{
	char msgId;
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
#define MESSAGE_READY 0x40	// lParam = ������ Ŭ���� ���̵�


//////////////////////////////////////////////
// Server to Clients
#define MESSAGE_YOUR_ID 0x00	// lParam = Ŭ���� ���̵�
#define MESSAGE_CONNECTED_IDS 0x01 // ������ ����� Ŭ���̾�Ʈ�� ID ��ȯ.
#define MESSAGE_GAME_START 0x02