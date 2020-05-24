#pragma once
#include "..\CyanEngine\framework.h"

class ParticleManager : public MonoBehavior<ParticleManager>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:
	ParticleSystem* particleSystem{ nullptr };

private:
	friend class GameObject;
	friend class MonoBehavior<ParticleManager>;
	ParticleManager() = default;
	ParticleManager(ParticleManager&) = default;

public:
	~ParticleManager() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
		particleSystem = gameObject->GetComponent<ParticleSystem>();
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
		if (Input::GetMouseButtonUp(0))
			if (particleSystem)
				particleSystem->enabled = !particleSystem->enabled;
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};