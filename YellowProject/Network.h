#pragma once
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#include "..\CyanEngine\framework.h"
#include <unordered_map>

#define BUFSIZE 200

using namespace std;

class Network : public MonoBehavior<Network>
{
private:
	GameObject* firstText{ nullptr };
	GameObject* secondText{ nullptr };

public:
	WSADATA WSAData;
	std::wstring wserverIp;
	std::wstring wmyname;
	std::wstring wchat;

	SOCKET serverSocket;
	int myId;
	int retval;

	bool setname{ false };
	bool isConnect{ false };
	bool tryConnect{ false };
	bool pressButton{ false };
	bool pressChatButton{ false };
	Text* chatText[4];
	Text* chatInputText{ nullptr };

	static Network* network;

public:
	GameObject* othersPrefab = NULL;
	GameObject* myCharacter = NULL;
	GameObject* npcsPrefab[10]{ nullptr };

	unordered_map<int, GameObject*> otherCharacters;

private:
	friend class GameObject;
	friend class MonoBehavior<Network>;
	Network() = default;
	Network(Network&) = default;

public:
	~Network() {
		isConnect = false;
		closesocket(serverSocket);
		WSACleanup();
	}
	void Update();
	void ProcessPacket(char* ptr);
	void process_data(char* net_buf, size_t io_byte);

	void Receiver();

	void send_packet(void* packet);
	void Login();
	void Logout();
	void send_move_packet(unsigned char dir);
	void send_attack_packet();
	void send_chat_packet(wchar_t msg[]);

	void Start()
	{
		WSAStartup(MAKEWORD(2, 0), &WSAData);

		firstText = Scene::scene->CreateUI();
		{
			auto rectTransform = firstText->GetComponent<RectTransform>();
			rectTransform->anchorMin = { 0, 1 };
			rectTransform->anchorMax = { 0, 1 };
			rectTransform->pivot = { 0, 1 };
			rectTransform->posX = 100;
			rectTransform->posY = -30;
			rectTransform->width = 100;
			rectTransform->height = 15;

			Text* text = firstText->AddComponent<Text>();
			text->text = L"";
			text->fontSize = 10;
			text->color = { 1.0f, 1.0f, 1.0f, 1.0f };
			text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
			text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			Scene::scene->textObjects.push_back(firstText);
		}
		firstText->SetActive(false);

		secondText = Scene::scene->CreateUI();
		{
			auto rectTransform = secondText->GetComponent<RectTransform>();
			rectTransform->anchorMin = { 0, 1 };
			rectTransform->anchorMax = { 0, 1 };
			rectTransform->pivot = { 0, 1 };
			rectTransform->posX = 200;
			rectTransform->posY = -30;
			rectTransform->width = 100;
			rectTransform->height = 15;

			Text* text = secondText->AddComponent<Text>();
			text->text = L"";
			text->fontSize = 10;
			text->color = { 0.0f, 0.0f, 0.0f, 1.0f };
			text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
			text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			Scene::scene->textObjects.push_back(secondText);
		}
		secondText->SetActive(false);
	}

	int connect_nonblock(SOCKET sockfd, const struct sockaddr FAR* name, int namelen, int timeout)
	{
		unsigned long nonzero = 1;
		unsigned long zero = 0;
		fd_set rset, wset;
		struct timeval tval;
		int n;
		int nfds = 1;

		if (ioctlsocket(sockfd, FIONBIO, &nonzero) == SOCKET_ERROR)
			return SOCKET_ERROR;

		if ((n = connect(sockfd, (struct sockaddr FAR*)name, namelen)) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
				return SOCKET_ERROR;
		}

		if (n == 0)
			goto done;

		FD_ZERO(&rset);
		FD_SET(sockfd, &rset);
		wset = rset;
		tval.tv_sec = timeout;
		tval.tv_usec = 0;

		if ((n = select(nfds, &rset, &wset, NULL, timeout ? &tval : NULL)) == 0)
		{
			WSASetLastError(WSAETIMEDOUT);
			return SOCKET_ERROR;
		}
	done:
		ioctlsocket(sockfd, FIONBIO, &zero);
		return 0;
	}

	void PressButton()
	{
		if (!isConnect && !tryConnect && !pressButton)
		{
			wmyname.clear();
			wserverIp.clear();
			firstText->GetComponent<Text>()->text = L"Input your Name : ";
			firstText->SetActive(true);
			secondText->GetComponent<Text>()->text = L"";
			secondText->SetActive(true);

			pressButton = true;
		}
		else if (pressButton)
		{
			setname = false;
			pressButton = false;
			wmyname.clear();
			wserverIp.clear();
			firstText->SetActive(false);
			secondText->SetActive(false);
		}
	}

	void addChat(wchar_t chatter[], wchar_t chat[])
	{
		for (int i = 2; i >= 0; --i)
		{
			chatText[i + 1]->text = chatText[i]->text;
		}
		wstring name = chatter;
		wstring msg = chat;
		chatText[0]->text = name + L": " + msg;
	}

	void PressChatButton()
	{
		if (isConnect)
		{
			if (pressChatButton)
				pressChatButton = false;
			else
				pressChatButton = true;
		}
	}
};