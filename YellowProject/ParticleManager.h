#pragma once
#include "..\CyanEngine\framework.h"

class ParticleManager : public MonoBehavior<ParticleManager>
{
private /*이 영역에 private 변수를 선언하세요.*/:

public  /*이 영역에 public 변수를 선언하세요.*/:
	ParticleSystem* particleSystem{ nullptr };

private:
	friend class GameObject;
	friend class MonoBehavior<ParticleManager>;
	ParticleManager() = default;
	ParticleManager(ParticleManager&) = default;

public:
	~ParticleManager() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
		particleSystem = gameObject->GetComponent<ParticleSystem>();
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		if (Input::GetMouseButtonUp(0))
			if (particleSystem)
				particleSystem->enabled = !particleSystem->enabled;
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};