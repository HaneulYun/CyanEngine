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

constexpr auto SCREEN_WIDTH = 8;
constexpr auto SCREEN_HEIGHT = 8;

constexpr auto TILE_WIDTH = 65;
constexpr auto WINDOW_WIDTH = TILE_WIDTH * SCREEN_WIDTH + 10;   // size of window
constexpr auto WINDOW_HEIGHT = TILE_WIDTH * SCREEN_WIDTH + 10;
constexpr auto BUF_SIZE = 200;
constexpr auto MAX_USER = 10;


class TCPClient : public MonoBehavior<TCPClient>
{
private /*이 영역에 private 변수를 선언하세요.*/:
	int NPC_ID_START = 10000;

	bool isIP{ false };
	std::wstring ip;

	WSADATA WSAData;
	SOCKET serverSocket;

	WSAOVERLAPPED recv;
	WSABUF recvbuf;

	char recvdata[BUF_SIZE];

	bool update_server{ 0 };

	DWORD flags{ 0 };

public  /*이 영역에 public 변수를 선언하세요.*/:
	GameObject* prefab;

	Pawn* avatar;
	std::unordered_map<int, Pawn*> npcs;

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

	void Start(/*초기화 코드를 작성하세요.*/)
	{
		std::wstring title(L"Yellow Project _ Input IP Here : ");

		SetWindowText(CyanApp::GetHwnd(), title.c_str());

		avatar = Scene::scene->Duplicate(prefab)->GetComponent<Pawn>();
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		if (!isIP)
		{
			for (char keyCode = '0'; keyCode <= '9'; ++keyCode)
			{
				if (Input::GetKeyDown((KeyCode)keyCode))
				{
					ip += keyCode;
					std::wstring title(L"Yellow Project _ Input IP Here : ");
					SetWindowText(CyanApp::GetHwnd(), (title + ip).c_str());
				}
			}
			if (Input::GetKeyDown(KeyCode::Period))
			{
				ip += '.';
				std::wstring title(L"Yellow Project _ Input IP Here : ");
				SetWindowText(CyanApp::GetHwnd(), (title + ip).c_str());
			}
			if (Input::GetKeyDown(KeyCode::Return))
			{
				isIP = true;

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
				int t_id = GetCurrentProcessId();
				sprintf(l_packet.name, "P%03d", t_id % 1000);
				strcpy(avatar->name, l_packet.name);
				//avatar->set_name(l_packet.name);
				send_packet(&l_packet);

				recv = {};
				recv.hEvent = (HANDLE)this;
				recvbuf.buf = (char*)&recvdata;
				recvbuf.len = BUF_SIZE;
				WSARecv(serverSocket, &recvbuf, 1, NULL, &flags, &recv, recv_callback);
			}
		}
		else
		{
			if (Input::GetKeyDown(KeyCode::A))
				send_move_packet(D_LEFT);
			else if (Input::GetKeyDown(KeyCode::D))
				send_move_packet(D_RIGHT);
			else if (Input::GetKeyDown(KeyCode::W))
				send_move_packet(D_UP);
			else if (Input::GetKeyDown(KeyCode::S))
				send_move_packet(D_DOWN);
		}
		if (update_server)
		{
			update_server = false;
			for (auto& d : npcs)
				d.second->SetPositionCurrentIndex();
		}
		SleepEx(10, TRUE);
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
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
	
		//MOVE_PACKET& buf = tcpClient->recvdata;

		//if (tcpClient->users.find(buf.id) == tcpClient->users.end() && tcpClient->pawns.size())
		//{
		//	tcpClient->allocPawns.push_back(tcpClient->pawns.back());
		//	tcpClient->pawns.pop_back();
		//	tcpClient->users[buf.id] = tcpClient->allocPawns.back();
		//}
		//tcpClient->update_server = true;
		//tcpClient->users[buf.id]->SetPositionIndex(buf.x, buf.y);
		//if (tcpClient->myID == -1)
		//	tcpClient->myID = buf.id;

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
			////g_left_x = my_packet->x - (SCREEN_WIDTH / 2);
			////g_top_y = my_packet->y - (SCREEN_HEIGHT / 2);
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
				if (id < NPC_ID_START)
					npcs[id] = Scene::scene->Duplicate(prefab)->GetComponent<Pawn>();
				else
					npcs[id] = Scene::scene->Duplicate(prefab)->GetComponent<Pawn>();
				strcpy(npcs[id]->name, my_packet->name);
				//npcs[id].set_name(my_packet->name);
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
				//g_left_x = my_packet->x - (SCREEN_WIDTH / 2);
				//g_top_y = my_packet->y - (SCREEN_HEIGHT / 2);
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
			//int other_id = my_packet->id;
			//if (other_id == g_myid) {
			//	avatar.hide();
			//}
			//else {
			//	if (0 != npcs.count(other_id))
			//		npcs[other_id].hide();
			//}
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

	void send_packet(void* packet)
	{
		char* p = reinterpret_cast<char*>(packet);
		send(serverSocket, p, p[0], 0);
	}

	void send_move_packet(unsigned char dir)
	{
		cs_packet_move m_packet;
		m_packet.type = C2S_MOVE;
		m_packet.size = sizeof(m_packet);
		m_packet.direction = dir;
		send_packet(&m_packet);
	}
};