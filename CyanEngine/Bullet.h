#pragma once
#include "framework.h"
#include "Message.h"

class Enemy;

class Bullet : public MonoBehavior<Bullet>
{
private /*�� ������ private ������ �����ϼ���.*/:
	

public  /*�� ������ public ������ �����ϼ���.*/:
	float timeCycle{ 0.5f };
	float speed{ 200.f };
	float lifeTime;
	float curLifeTime;
	Vector3 direction;

private:
	friend class GameObject;
	friend class MonoBehavior<Bullet>;
	Bullet() = default;
	Bullet(Bullet&) = default;

public:
	~Bullet() {}

	void CheckOutOfRange()
	{
		// Delete Bullet Out of Range
		Vector3 distance = gameObject->transform->position - Vector3{ 0.0f,0.0f,0.0f };
		if (distance.Length() >= 200.0f)
		{
			Message message;
			message.msgId = MESSAGE_DELETE_BULLET;
			message.lParam = gameObject->GetComponent<ObjectID>()->GetObjectID();
			EnterCriticalSection(&rqcs);
			recvQueue.push(message);
			LeaveCriticalSection(&rqcs);
		}
	}

	void UpdateLifeTime()
	{
		curLifeTime -= Time::deltaTime;
		if (curLifeTime <= 0.0f)
		{
			Message message;
			message.msgId = MESSAGE_DELETE_BULLET;
			message.lParam = gameObject->GetComponent<ObjectID>()->GetObjectID();
			EnterCriticalSection(&rqcs);
			recvQueue.push(message);
			LeaveCriticalSection(&rqcs);
		}
	}

	void Start()
	{
		// �ʱ�ȭ �ڵ带 �ۼ��ϼ���.
		direction = direction.Normalize();
		gameObject->transform->Rotate({ 0, 0, 1 }, direction.Degree() - 90);

		if (speed != 0)
		{
			GameObject* vfx = new GameObject();
			*vfx->GetComponent<Transform>() = *gameObject->GetComponent<Transform>();
			vfx->GetComponent<Transform>()->gameObject = vfx;
			vfx->AddComponent<MeshFilter>(gameObject->GetComponent<MeshFilter>());
			vfx->AddComponent<Renderer>(gameObject->GetComponent<Renderer>());
			vfx->AddComponent<EmptyShell>()->SetEntity(-direction, 1);
		}
	}

	void Update()
	{
		Vector3 movevector = direction * speed * Time::deltaTime;
		gameObject->transform->position = gameObject->transform->position + movevector;

		CheckOutOfRange();
		if (speed == 0) UpdateLifeTime();
	}

	void OnTriggerEnter(GameObject* collision)
	{
		//if (!collision->GetComponent<Enemy>())
		//	return;
		//Destroy(gameObject);
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
	void SetEntity(const float _timeCycle, const float _speed, const float _lifeTime = {}, const Vector3& _direction = {})
	{
		timeCycle = _timeCycle;
		speed = _speed;
		direction = _direction;
		lifeTime = _lifeTime;
		curLifeTime = lifeTime;
	}
};