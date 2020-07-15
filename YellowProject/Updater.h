#pragma once
#include "..\CyanEngine\framework.h"

class Updater : public MonoBehavior<Updater>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:
	void* dst{ nullptr };
	void* src{ nullptr };
	int size{ 0 };

protected:
	friend class GameObject;
	friend class MonoBehavior<Updater>;
	Updater() = default;
	Updater(Updater&) = default;

public:
	~Updater() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
		memcpy(dst, src, size);
	}

	void Set(void* dst, void* src, int size)
	{
		this->dst = dst;
		this->src = src;
		this->size = size;
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};