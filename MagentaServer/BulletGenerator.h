#pragma once

class BulletGenerator : public MonoBehavior<BulletGenerator>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:
	GameObject* bullet[5]{ nullptr };
	ObjectIDManager* objIDmgr{ nullptr };

private:
	friend class GameObject;
	friend class MonoBehavior<BulletGenerator>;
	BulletGenerator() = default;
	BulletGenerator(BulletGenerator&) = default;

public:
	~BulletGenerator() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void CreateBullet(int type, int playerID, float radian)
	{
		if (SceneManager::scenemanager->GetComponent<SceneManager>()->gameState == START) 
		{
			GameObject* object = Instantiate(bullet[type]);
			//{
			//	object->transform->position = gameObject->transform->position;
			//
			//	Vector3 axis{ 0.0f, 0.0f, 1.0f };
			//	Vector3 v{ 0.0f, 1.0f, 0.0f };
			//	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&axis.xmf3), XMConvertToRadians(radian));
			//	
			//	object->GetComponent<Bullet>()->direction = NS_Vector3::TransformCoord(v.xmf3, mtxRotate);
			//}
			
		}
	}
	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

