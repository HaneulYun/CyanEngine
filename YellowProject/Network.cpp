#include "pch.h"
#include "Network.h"

Network* Network::network{ nullptr };

void Network::ProcessPacket(char* ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case S2C_LOGIN_OK:
	{
		sc_packet_login_ok* my_packet = reinterpret_cast<sc_packet_login_ok*>(ptr);
		myId = my_packet->id;
		myCharacter->transform->position = { my_packet->x * 0.055f, -my_packet->y * 0.055f, -0.0001f };
		CharacterController* controller = myCharacter->GetComponent<CharacterController>();
		controller->xPos = my_packet->x;
		controller->yPos = my_packet->y;
	}
	break;

	case S2C_ENTER:
	{
		sc_packet_enter* my_packet = reinterpret_cast<sc_packet_enter*>(ptr);
		int id = my_packet->id;

		if (id == myId) {
			myCharacter->transform->position = { my_packet->x * 0.055f, -my_packet->y * 0.055f, -0.0001f };
			CharacterController* controller = myCharacter->GetComponent<CharacterController>();
			strcpy_s(controller->name, my_packet->name);
			controller->setName();
			controller->xPos = my_packet->x;
			controller->yPos = my_packet->y;
		}
		else if(id < NPC_ID_START){
			otherCharacters[id] = gameObject->scene->Duplicate(othersPrefab);
			otherCharacters[id]->transform->position = { my_packet->x * 0.055f, -my_packet->y * 0.055f, -0.0001f };
			CharacterController* controller = otherCharacters[id]->GetComponent<CharacterController>();
			strcpy_s(controller->name, my_packet->name);
			controller->setName();
			controller->xPos = my_packet->x;
			controller->yPos = my_packet->y;
			int x = controller->xPos - myCharacter->GetComponent<CharacterController>()->xPos + 10;
			int y = controller->yPos - myCharacter->GetComponent<CharacterController>()->yPos + 10;
			controller->renewTextPos(x, y);
		}
		else
		{
			otherCharacters[id] = gameObject->scene->Duplicate(npcsPrefab);
			otherCharacters[id]->transform->position = { my_packet->x * 0.055f, -my_packet->y * 0.055f, -0.0001f };
			CharacterController* controller = otherCharacters[id]->GetComponent<CharacterController>();
			strcpy_s(controller->name, my_packet->name);
			controller->setName();
			controller->xPos = my_packet->x;
			controller->yPos = my_packet->y;
			int x = controller->xPos - myCharacter->GetComponent<CharacterController>()->xPos + 10;
			int y = controller->yPos - myCharacter->GetComponent<CharacterController>()->yPos + 10;
			controller->renewTextPos(x, y);
		}
	}
	break;
	case S2C_MOVE:
	{
		sc_packet_move* my_packet = reinterpret_cast<sc_packet_move*>(ptr);
		int other_id = my_packet->id;
		if (other_id == myId) {
			myCharacter->transform->position = { my_packet->x * 0.055f, -my_packet->y * 0.055f, -0.0001f };
			CharacterController* controller = myCharacter->GetComponent<CharacterController>();
			controller->xPos = my_packet->x;
			controller->yPos = my_packet->y;
		}
		else {
			if (0 != otherCharacters.count(other_id))
			{
				otherCharacters[other_id]->transform->position = { my_packet->x * 0.055f, -my_packet->y * 0.055f, -0.0001f };
				CharacterController* controller = otherCharacters[other_id]->GetComponent<CharacterController>();
				controller->xPos = my_packet->x;
				controller->yPos = my_packet->y;
				int x = controller->xPos - myCharacter->GetComponent<CharacterController>()->xPos + 10;
				int y = controller->yPos - myCharacter->GetComponent<CharacterController>()->yPos + 10;
				controller->renewTextPos(x, y);
			}
		}
	}
	break;

	case S2C_LEAVE:
	{
		sc_packet_leave* my_packet = reinterpret_cast<sc_packet_leave*>(ptr);
		int other_id = my_packet->id;

		if (other_id != myId) {
			if (0 != otherCharacters.count(other_id))
			{
				Scene::scene->PushDelete(otherCharacters[other_id]);
				otherCharacters.erase(other_id);
			}
		}
	}
	break;

	case S2C_CHAT:
	{
		sc_packet_chat* my_packet = reinterpret_cast<sc_packet_chat*>(ptr);
		int id = my_packet->id;

		if (id == myId) {
			myCharacter->GetComponent<CharacterController>()->addChat(my_packet->mess);
		}
		else {
			CharacterController* controller = otherCharacters[id]->GetComponent<CharacterController>();
			int x = controller->xPos - myCharacter->GetComponent<CharacterController>()->xPos + 10;
			int y = controller->yPos - myCharacter->GetComponent<CharacterController>()->yPos + 10;
			controller->renewTextPos(x, y);
			otherCharacters[id]->GetComponent<CharacterController>()->addChat(my_packet->mess);
		}
	}
	break;

	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);
	}
}

void Network::process_data(char* net_buf, size_t io_byte)
{
	char* ptr = net_buf;
	static size_t in_packet_size = 0;
	static size_t saved_packet_size = 0;
	static char packet_buffer[BUFSIZE];

	while (0 != io_byte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (io_byte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			io_byte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
			saved_packet_size += io_byte;
			io_byte = 0;
		}
	}
}

void Network::Receiver()
{
	char net_buf[BUFSIZE];
	auto retval = recv(serverSocket, net_buf, BUFSIZE, 0);

	if (retval > 0)	process_data(net_buf, retval);
}

void Network::send_packet(void* packet)
{
	char* p = reinterpret_cast<char*>(packet);
	send(serverSocket, p, p[0], 0);
}

void Network::send_move_packet(unsigned char dir)
{
	cs_packet_move m_packet;
	m_packet.type = C2S_MOVE;
	m_packet.size = sizeof(m_packet);
	m_packet.direction = dir;

	send_packet(&m_packet);
}

void Network::Login()
{
	cs_packet_login l_packet;
	l_packet.size = sizeof(l_packet);
	l_packet.type = C2S_LOGIN;
	strcpy_s(l_packet.name, myCharacter->GetComponent<CharacterController>()->name);

	send_packet(&l_packet);
}