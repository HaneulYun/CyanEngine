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
		// �ʱ�ȭ �ڵ带 �ۼ��ϼ���.
	}

	void Update()
	{
		
	}

	void OnTriggerEnter(GameObject* collision)
	{
		printf("��\n");
		//if (collision->GetComponent<Damageable>() != NULL)
		//{
		//	gameObject->GetComponent<Damager>()->AddDamageTo(*collision->GetComponent<Damageable>());
		//}
	}
};