#pragma once
#include "framework.h"

class TCPClient : public MonoBehavior<TCPClient>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:

private:
	friend class GameObject;
	friend class MonoBehavior<TCPClient>;
	TCPClient() = default;
	TCPClient(TCPClient&) = default;

public:
	~TCPClient() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};