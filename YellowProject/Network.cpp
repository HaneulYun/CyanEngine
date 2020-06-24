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
	case S2C_LOGIN_FAIL:
	{
		sc_packet_login_fail* my_packet = reinterpret_cast<sc_packet_login_fail*>(ptr);
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
			if (controller)
			{
				strcpy_s(controller->name, my_packet->name);
				controller->setName();
				controller->xPos = my_packet->x;
				controller->yPos = my_packet->y;
				int x = controller->xPos - myCharacter->GetComponent<CharacterController>()->xPos + 10;
				int y = controller->yPos - myCharacter->GetComponent<CharacterController>()->yPos + 10;
				controller->renewTextPos(x, y);
			}
		}
		else
		{
			otherCharacters[id] = gameObject->scene->Duplicate(npcsPrefab);
			otherCharacters[id]->transform->position = { my_packet->x * 0.055f, -my_packet->y * 0.055f, -0.0001f };
			CharacterController* controller = otherCharacters[id]->GetComponent<CharacterController>();
			if (controller)
			{
				strcpy_s(controller->name, my_packet->name);
				controller->setName();
				controller->xPos = my_packet->x;
				controller->yPos = my_packet->y;
				int x = controller->xPos - myCharacter->GetComponent<CharacterController>()->xPos + 10;
				int y = controller->yPos - myCharacter->GetComponent<CharacterController>()->yPos + 10;
				controller->renewTextPos(x, y);
			}
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
			for (auto oc : otherCharacters)
			{
				CharacterController* occontroller = oc.second->GetComponent<CharacterController>();
				if (occontroller)
				{
					int x = occontroller->xPos - controller->xPos + 10;
					int y = occontroller->yPos - controller->yPos + 10;
					occontroller->renewTextPos(x, y);
				}
			}
		}
		else {
			if (0 != otherCharacters.count(other_id))
			{
				otherCharacters[other_id]->transform->position = { my_packet->x * 0.055f, -my_packet->y * 0.055f, -0.0001f };
				CharacterController* controller = otherCharacters[other_id]->GetComponent<CharacterController>();
				if (controller)
				{
					controller->xPos = my_packet->x;
					controller->yPos = my_packet->y;
					int x = controller->xPos - myCharacter->GetComponent<CharacterController>()->xPos + 10;
					int y = controller->yPos - myCharacter->GetComponent<CharacterController>()->yPos + 10;
					controller->renewTextPos(x, y);
				}
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
		if (id == myId)
		{
			CharacterController* controller = myCharacter->GetComponent<CharacterController>();
			wstring name;
			string cname = controller->name;
			name.assign(cname.begin(), cname.end());
			addChat(_wcsdup(name.c_str()), my_packet->mess);
		}
		else
		{
			CharacterController* controller = otherCharacters[id]->GetComponent<CharacterController>();
			wstring name;
			string cname = controller->name;
			name.assign(cname.begin(), cname.end());
			addChat(_wcsdup(name.c_str()), my_packet->mess);
		}
	}
	break;

	case S2C_STAT_CHANGE:
	{
		sc_packet_stat_change* my_packet = reinterpret_cast<sc_packet_stat_change*>(ptr);
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
		if (0 == in_packet_size) in_packet_size = (unsigned char)ptr[0];
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
	send(serverSocket, p, (unsigned char)p[0], 0);
}

void Network::send_move_packet(unsigned char dir)
{
	cs_packet_move m_packet;
	m_packet.type = C2S_MOVE;
	m_packet.size = sizeof(m_packet);
	m_packet.direction = dir;

	send_packet(&m_packet);
}

void Network::send_attack_packet()
{
	cs_packet_attack m_packet;
	m_packet.type = C2S_ATTACK;
	m_packet.size = sizeof(m_packet);

	send_packet(&m_packet);
}

void Network::send_chat_packet(wchar_t msg[])
{
	cs_packet_chat m_packet;
	m_packet.type = C2S_CHAT;
	m_packet.size = sizeof(m_packet);
	wcscpy_s(m_packet.message, msg);

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

void Network::Logout()
{
	cs_packet_logout l_packet;
	l_packet.size = sizeof(l_packet);
	l_packet.type = C2S_LOGOUT;

	send_packet(&l_packet);
}

void Network::Update()
{
	if (!isConnect && !tryConnect && pressButton)
	{
		for (char key = '0'; key <= '9'; ++key)
		{
			if (Input::GetKeyDown((KeyCode)key))
			{
				if (!setname)
				{
					wmyname += key;
					secondText->GetComponent<Text>()->text = wmyname;
				}
				else
				{
					wserverIp += key;
					secondText->GetComponent<Text>()->text = wserverIp;
				}
			}
		}
		for (char key = 'A'; key <= 'Z'; ++key)
		{
			if (Input::GetKeyDown((KeyCode)key))
			{
				if (!setname)
				{
					wmyname += key;
					secondText->GetComponent<Text>()->text = wmyname;
				}
			}
		}
		if (Input::GetKeyDown(KeyCode::Period))
		{
			if (!setname)
			{
				wmyname += '.';
				secondText->GetComponent<Text>()->text = wmyname;
			}
			else
			{
				wserverIp += '.';
				secondText->GetComponent<Text>()->text = wserverIp;
			}
		}
		if (Input::GetKeyDown(KeyCode::Shift))
		{
			if (!setname)
			{
				if (!wmyname.empty())
					wmyname.pop_back();
				secondText->GetComponent<Text>()->text = wmyname;
			}
			else
			{
				if (!wserverIp.empty())
					wserverIp.pop_back();
				secondText->GetComponent<Text>()->text = wserverIp;
			}
		}
		if (Input::GetKeyDown(KeyCode::Return))
		{
			if (!setname)
			{
				firstText->GetComponent<Text>()->text = L"Input Server IP : ";
				secondText->GetComponent<Text>()->text = L"";
				string myname;
				myname.assign(wmyname.begin(), wmyname.end());
				strncpy(myCharacter->GetComponent<CharacterController>()->name, myname.c_str(), myname.length());
				myCharacter->GetComponent<CharacterController>()->setName();
				setname = true;
			}
			else
			{
				std::string serverIp;
				serverIp.assign(wserverIp.begin(), wserverIp.end());

				SOCKADDR_IN serveraddr{};
				serveraddr.sin_family = AF_INET;
				serveraddr.sin_addr.s_addr = inet_addr(serverIp.c_str());
				serveraddr.sin_port = htons(SERVER_PORT);

				retval = connect_nonblock(serverSocket, (SOCKADDR*)&serveraddr, sizeof(serveraddr), 5);

				tryConnect = true;
				pressButton = false;

				firstText->SetActive(false);
				secondText->SetActive(false);

				wserverIp.clear();
			}
		}
	}
	if (isConnect && pressChatButton)
	{
		for (char key = '0'; key <= '9'; ++key)
		{
			if (Input::GetKeyDown((KeyCode)key))
			{
				wchat += key;
				chatInputText->text = wchat;
			}
		}
		for (char key = 'A'; key <= 'Z'; ++key)
		{
			if (Input::GetKeyDown((KeyCode)key))
			{
				wchat += key;
				chatInputText->text = wchat;
			}
		}
		if (Input::GetKeyDown(KeyCode::Period))
		{
			wchat += '.';
			chatInputText->text = wchat;
		}
		if (Input::GetKeyDown(KeyCode::Shift))
		{
			if (!wchat.empty())
				wchat.pop_back();
			chatInputText->text = wchat;
		}
		if (Input::GetKeyDown(KeyCode::Return))
		{
			send_chat_packet(_wcsdup(wchat.c_str()));
			wchat.clear();
			chatInputText->text = wchat;
			pressChatButton = false;
		}
	}
	if (tryConnect)
	{
		if (retval == SOCKET_ERROR)
		{
			tryConnect = false;
			isConnect = false;
			setname = false;
			wmyname.clear();
			wserverIp.clear();
		}
		else if (retval == 0)
		{
			tryConnect = false;
			isConnect = true;
			wserverIp.clear();
			unsigned long on = true;
			int nRet = ioctlsocket(serverSocket, FIONBIO, &on);
			Login();
			wmyname.clear();
		}
	}
	if (isConnect)
	{
		Receiver();
	}
}