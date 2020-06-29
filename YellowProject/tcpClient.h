#pragma once
#pragma comment(lib, "ws2_32")
#include <WS2tcpip.h>
#include "framework.h"
#include "..\CyanServer\protocol.h"

#define SERVER_IP	"127.0.0.1"

//struct MOVE_PACKET
//{
//	char id; char x; char y;
//};

constexpr auto SCREEN_WIDTH = 20;
constexpr auto SCREEN_HEIGHT = 20;

constexpr auto TILE_WIDTH = 65;
constexpr auto WINDOW_WIDTH = TILE_WIDTH * SCREEN_WIDTH / 2 + 10;   // size of window
constexpr auto WINDOW_HEIGHT = TILE_WIDTH * SCREEN_WIDTH / 2 + 10;
constexpr auto BUF_SIZE = 200;
constexpr auto MAX_USER = NPC_ID_START;


class TCPClient : public MonoBehavior<TCPClient>
{
private /*�� ������ private ������ �����ϼ���.*/:
	char map[WORLD_HEIGHT][WORLD_WIDTH];

	//int NPC_ID_START = 10000;
	bool isIP{ false };
	std::wstring ip;

	bool isID{ false };
	std::wstring id;

	bool isChatting{ false };

	WSADATA WSAData;
	SOCKET serverSocket;

	WSAOVERLAPPED recv;
	WSABUF recvbuf;

	char recvdata[BUF_SIZE];

	bool update_server{ 0 };

	DWORD flags{ 0 };

public  /*�� ������ public ������ �����ϼ���.*/:
	GameObject* tiles[SCREEN_HEIGHT][SCREEN_WIDTH];
	GameObject* prefab;

	Pawn* avatar;
	std::unordered_map<int, Pawn*> npcs;

	Text* coordinateText;
	Text* statusText;
	Text* chatter;
	Text* logger;
	GameObject* ipEditor{ nullptr };
	GameObject* idEditor{ nullptr };

	std::queue<std::wstring> chatlog;
	//char myID{ -1 };
	int g_left_x;
	int g_top_y;
	int g_myid;

private:
	friend class GameObject;
	friend class MonoBehavior<TCPClient>;
	TCPClient() = default;
	TCPClient(TCPClient&) = default;

public:
	~TCPClient()
	{
		closesocket(serverSocket);
		WSACleanup();
	}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
		avatar = Scene::scene->Duplicate(prefab)->GetComponent<Pawn>();
		std::ifstream in;
		in.open("tilemap.data", std::ios::binary);
		in.read((char*)map, 800 * 800);
		in.close();
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
		if (!InputID());
		else if (!InputIP());
		else
		{
			if (Chatting());
			else if (Input::GetKeyDown(KeyCode::Left))
				send_move_packet(D_LEFT);
			else if (Input::GetKeyDown(KeyCode::Right))
				send_move_packet(D_RIGHT);
			else if (Input::GetKeyDown(KeyCode::Up))
				send_move_packet(D_UP);
			else if (Input::GetKeyDown(KeyCode::Down))
				send_move_packet(D_DOWN);
			else if (Input::GetKeyDown(KeyCode::A))
				send_attack_packet();

			for (int i = 0; i < SCREEN_WIDTH; ++i)
				for (int j = 0; j < SCREEN_HEIGHT; ++j)
				{
					int tile_x = i + g_left_x;
					int tile_y = j + g_top_y;

					auto tile = tiles[j][i];
					//tile->transform->position = { (float)i, (float)j, 0 };

					if ((tile_x < 0) || (tile_y < 0))
					{
						tile->GetComponent<Renderer>()->materials[0] = ASSET MATERIAL("none");
						continue;
					}
					//
					//if ((tile_x / 3 + tile_y / 3) % 2)
					////if ((tile_x + tile_y) % 2)
					//	tile->GetComponent<Renderer>()->materials[0] = ASSET MATERIAL("whiteTileMat");
					//else
					//	tile->GetComponent<Renderer>()->materials[0] = ASSET MATERIAL("blackTileMat");
					Material* mat = ASSET MATERIAL(std::to_string((unsigned char)map[800-tile_y-1][tile_x]-1));
					if (!mat)
						int k = 0;
					tile->GetComponent<Renderer>()->materials[0] = mat;
				}

			float x = (float)avatar->x - g_left_x;
			float y = (float)avatar->y - g_top_y;
			avatar->gameObject->transform->position = { (float)avatar->x - g_left_x, (float)avatar->y - g_top_y, -1 };
			auto rect = avatar->gameObject->children[0]->GetComponent<RectTransform>();
			rect->anchorMin = { (x + 0.5f) / 20, (y + 0.5f) / 20 };
			rect->anchorMax = { (x + 0.5f) / 20, (y + 0.5f) / 20 };

			rect = avatar->gameObject->children[1]->GetComponent<RectTransform>();
			rect->anchorMin = { (x + 0.5f) / 20, (y) / 20 };
			rect->anchorMax = { (x + 0.5f) / 20, (y) / 20 };
			for (auto& d : npcs)
			{
				float x = (float)d.second->x - g_left_x;
				float y = (float)d.second->y - g_top_y;
				d.second->gameObject->transform->position = { x, y, -1 };
				auto rect = d.second->gameObject->children[0]->GetComponent<RectTransform>();
				rect->anchorMin = { (x+0.5f) / 20, (y+0.5f) / 20 };
				rect->anchorMax = { (x+0.5f) / 20, (y+0.5f) / 20 };

				rect = d.second->gameObject->children[1]->GetComponent<RectTransform>();
				rect->anchorMin = { (x + 0.5f) / 20, (y) / 20 };
				rect->anchorMax = { (x + 0.5f) / 20, (y) / 20 };
			}

			wchar_t wstr[20];
			wsprintf(wstr, L"(%d, %d)", avatar->x, avatar->y);
			coordinateText->text = wstr;

			avatar->UpdateStatus(statusText->text);
		}
		SleepEx(10, TRUE);
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
	int GetIndexFromPosition(float d)
	{
		return floorf((d + 300) / 75);
	}

	static void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
	{
		TCPClient* tcpClient = reinterpret_cast<TCPClient*>(overlapped->hEvent);
		SOCKET client_s = tcpClient->serverSocket;
	
		if (dataBytes == 0)
		{
			closesocket(client_s);
			return;
		}

		if (dataBytes > 0) tcpClient->process_data(tcpClient->recvdata, dataBytes);
	
		DWORD flags{ 0 };
		tcpClient->recv.hEvent = (HANDLE)tcpClient;
		tcpClient->recvbuf.buf = (char*)&tcpClient->recvdata;
		tcpClient->recvbuf.len = BUF_SIZE;
		WSARecv(tcpClient->serverSocket, &tcpClient->recvbuf, 1, NULL, &flags, &tcpClient->recv, recv_callback);
	}

	void ProcessPacket(char* ptr)
	{
		static bool first_time = true;
		switch (ptr[1])
		{
		case S2C_LOGIN_OK:
		{
			sc_packet_login_ok* my_packet = reinterpret_cast<sc_packet_login_ok*>(ptr);
			g_myid = my_packet->id;
			avatar->move(my_packet->x, my_packet->y);
			g_left_x = my_packet->x - (SCREEN_WIDTH / 2);
			g_top_y = my_packet->y - (SCREEN_HEIGHT / 2);
			//avatar.show();
		}
		break;

		case S2C_ENTER:
		{
			sc_packet_enter* my_packet = reinterpret_cast<sc_packet_enter*>(ptr);
			int id = my_packet->id;

			if (id == g_myid) {
				avatar->move(my_packet->x, my_packet->y);
				//g_left_x = my_packet->x - (SCREEN_WIDTH / 2);
				//g_top_y = my_packet->y - (SCREEN_HEIGHT / 2);
				//avatar.show();
			}
			else {
				npcs[id] = Scene::scene->Duplicate(prefab)->GetComponent<Pawn>();
				if (id < NPC_ID_START)
					npcs[id]->gameObject->GetComponent<Renderer>()->materials[0] = ASSET MATERIAL("userMat");
				else
				{
					switch (my_packet->o_type)
					{
					case O_HUMAN:
						npcs[id]->gameObject->GetComponent<Renderer>()->materials[0] = ASSET MATERIAL("humanMat");
						break;
					case O_ELF:
						npcs[id]->gameObject->GetComponent<Renderer>()->materials[0] = ASSET MATERIAL("elfMat");
						break;
					case O_ORC:
						npcs[id]->gameObject->GetComponent<Renderer>()->materials[0] = ASSET MATERIAL("orcMat");
						break;
					}
				}
				strcpy(npcs[id]->name, my_packet->name);
				npcs[id]->setName();
				npcs[id]->move(my_packet->x, my_packet->y);
				//npcs[id].show();
			}
		}
		break;
		case S2C_MOVE:
		{
			sc_packet_move* my_packet = reinterpret_cast<sc_packet_move*>(ptr);
			int other_id = my_packet->id;
			if (other_id == g_myid) {
				avatar->move(my_packet->x, my_packet->y);
				g_left_x = my_packet->x - (SCREEN_WIDTH / 2);
				g_top_y = my_packet->y - (SCREEN_HEIGHT / 2);
			}
			else {
				if (0 != npcs.count(other_id))
					npcs[other_id]->move(my_packet->x, my_packet->y);
			}
		}
		break;
		case S2C_LEAVE:
		{
			sc_packet_leave* my_packet = reinterpret_cast<sc_packet_leave*>(ptr);
			int other_id = my_packet->id;
			if (other_id == g_myid) {
				avatar->gameObject->SetActive(false);//.hide();
			}
			else {
				if (0 != npcs.count(other_id))
					npcs[other_id]->gameObject->SetActive(false); //.hide();
			}
		}
		break;
		case S2C_CHAT:
		{
			sc_packet_chat* my_packet = reinterpret_cast<sc_packet_chat*>(ptr);
			int o_id = my_packet->id;
			if (0 != npcs.count(o_id))
			{
				npcs[o_id]->addChat(my_packet->mess);
			}

			if (o_id < NPC_ID_START)
			{
				chatlog.push(my_packet->mess);
				UpdateLog();
			}
		}
		break;
		case S2C_STAT_CHANGE:
		{
			sc_packet_stat_change* my_packet = reinterpret_cast<sc_packet_stat_change*>(ptr);
			
			avatar->hp = my_packet->hp;
			avatar->level = my_packet->level;
			avatar->exp = my_packet->exp;
		}
		break;
		default:
			printf("Unknown PACKET type [%d]\n", ptr[1]);

		}
	}

	void process_data(char* net_buf, size_t io_byte)
	{
		char* ptr = net_buf;
		static size_t in_packet_size = 0;
		static size_t saved_packet_size = 0;
		static char packet_buffer[BUF_SIZE];

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

	void send_packet(void* packet)
	{
		char* p = reinterpret_cast<char*>(packet);
		send(serverSocket, p, p[0], 0);
	}

	void send_move_packet(unsigned char dir)
	{
		cs_packet_move m_packet{};
		m_packet.type = C2S_MOVE;
		m_packet.size = sizeof(m_packet);
		m_packet.direction = dir;
		send_packet(&m_packet);
	}

	void send_attack_packet()
	{
		cs_packet_attack a_packet{};
		a_packet.type = C2S_ATTACK;
		a_packet.size = sizeof(a_packet);
		send_packet(&a_packet);
	}

	void send_chat_packet(const std::wstring& message)
	{
		cs_packet_chat c_packet{};
		c_packet.type = C2S_CHAT;
		c_packet.size = sizeof(c_packet);
		wcscpy(c_packet.message, message.c_str());
		send_packet(&c_packet);
	}

	void UpdateLog()
	{
		while (chatlog.size() > 6)
			chatlog.pop();
		logger->text = L"";

		auto logtmp = chatlog;
		while (logtmp.size())
		{
			logger->text += L"\n" + logtmp.front();
			logtmp.pop();
		}
	}

	bool Chatting()
	{
		if (!isChatting)
		{
			if (Input::GetKeyDown(KeyCode::Return))
			{
				isChatting = true;
				Input::ClearBuffer();
			}
		}
		else
		{
			chatter->text = id + L" : " + Input::buffer;

			if (Input::GetKeyDown(KeyCode::Return))
			{
				chatlog.push(chatter->text);
				send_chat_packet(chatter->text);
				UpdateLog();
				chatter->text = L"";
				isChatting = false;
				return true;
			}
		}
		return isChatting;
	}

	bool InputID()
	{
		if (!isID)
		{
			if (Input::GetKeyDown(KeyCode::Return))
			{
				isID = true;
				idEditor->SetActive(false);
				Input::ClearBuffer();
				return false;
			}
			else
			{
				wchar_t wstr[256];
				wsprintf(wstr, L"%s", Input::buffer);
				id = wstr;
				std::wstring idis(L"ID : ");
				idEditor->GetComponent<Text>()->text = idis + wstr;
			}
		}
		return isID;
	}

	bool InputIP()
	{
		if (!isIP)
		{
			if (Input::GetKeyDown(KeyCode::Return))
			{
				isIP = true;
				ipEditor->SetActive(false);
				Input::ClearBuffer();

				WSAStartup(MAKEWORD(2, 0), &WSAData);
				serverSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

				std::string addr;
				addr.assign(ip.begin(), ip.end());

				SOCKADDR_IN serverAddr{};
				serverAddr.sin_family = AF_INET;
				serverAddr.sin_port = htons(SERVER_PORT);
				inet_pton(AF_INET, addr.c_str(), &serverAddr.sin_addr);
				WSAConnect(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr), 0, 0, 0, 0);

				cs_packet_login l_packet;
				l_packet.size = sizeof(l_packet);
				l_packet.type = C2S_LOGIN;
				sprintf(l_packet.name, "%ls", id.c_str());
				strcpy(avatar->name, l_packet.name);
				avatar->setName();// set_name(l_packet.name);
				send_packet(&l_packet);

				recv = {};
				recv.hEvent = (HANDLE)this;
				recvbuf.buf = (char*)&recvdata;
				recvbuf.len = BUF_SIZE;
				WSARecv(serverSocket, &recvbuf, 1, NULL, &flags, &recv, recv_callback);
				return false;
			}
			else
			{
				wchar_t wstr[256];
				wsprintf(wstr, L"%s", Input::buffer);
				ip = wstr;
				std::wstring ipis(L"IP : ");
				ipEditor->GetComponent<Text>()->text = ipis + wstr;
			}
		}
		return isIP;
	}
};