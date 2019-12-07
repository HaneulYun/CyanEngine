#pragma once

class BulletGenerator : public MonoBehavior<BulletGenerator>
{
private /*이 영역에 private 변수를 선언하세요.*/:

public  /*이 영역에 public 변수를 선언하세요.*/:
	GameObject* bullet[5]{ nullptr };
	ObjectIDManager* objIDmgr{ nullptr };

private:
	friend class GameObject;
	friend class MonoBehavior<BulletGenerator>;
	BulletGenerator() = default;
	BulletGenerator(BulletGenerator&) = default;

public:
	~BulletGenerator() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
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
	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};

