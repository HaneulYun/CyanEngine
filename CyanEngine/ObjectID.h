#pragma once
#include "framework.h"

class ObjectID : public MonoBehavior<ObjectID>
{
private /*�� ������ private ������ �����ϼ���.*/:
	int objectid;
public  /*�� ������ public ������ �����ϼ���.*/:

private:
	friend class GameObject;
	friend class MonoBehavior<ObjectID>;
	ObjectID() = default;
	ObjectID(int id) : objectid(id) {};
	ObjectID(ObjectID&) = default;

public:
	~ObjectID() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void SetObjectID(int id)
	{
		objectid = id;
	}
	int GetObjectID()
	{
		return objectid;
	}
	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

