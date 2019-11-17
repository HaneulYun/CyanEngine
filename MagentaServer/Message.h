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
#define ADD 0x0041
#define SUB 0x0042
// Server to Clients
#define EQUAL 0x0045

//////////////////////////////////////////////
// Client to Server
#define MESSAGE_READY 0x1000	// lParam = ������ Ŭ���� ���̵�


//////////////////////////////////////////////
// Server to Clients
#define MESSAGE_YOUR_ID 0x0000	// lParam = Ŭ���� ���̵�
#define MESSAGE_CONNECTED_IDS 0x0001 // ������ ����� Ŭ���̾�Ʈ�� ID ��ȯ.
#define MESSAGE_GAME_START 0x0002