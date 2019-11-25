#pragma once

class Star : public Component
{
private:
	friend class GameObject;
	Star() {}

public:
	~Star() {}
	virtual Component* Duplicate() { return new Star; };

	void Start()
	{
		// 초기화 코드를 작성하세요.
	}

	void Update()
	{
		
	}

	void OnTriggerEnter(GameObject* collision)
	{
		printf("먹\n");
		//if (collision->GetComponent<Damageable>() != NULL)
		//{
		//	gameObject->GetComponent<Damager>()->AddDamageTo(*collision->GetComponent<Damageable>());
		//}
	}
};