#pragma once

class Animator : public MonoBehavior<Animator>
{
public:
	AnimatorController* controller;

public:
	std::vector<DirectX::XMFLOAT4X4> FinalTransforms;
	std::string ClipName;
	float TimePos = 0.0f;

	void UpdateSkinnedAnimation(float dt)
	{
		TimePos += dt;

		float k = controller->GetClipEndTime(ClipName);

		if (TimePos > controller->GetClipEndTime(ClipName))
			TimePos = 0.0f;

		controller->GetFinalTransforms(ClipName, TimePos, FinalTransforms);
	}

	UINT SkinnedCBIndex = -1;

private:
	friend class GameObject;
	friend class MonoBehavior<Animator>;
	Animator() = default;
	Animator(Animator&) = default;

public:
	~Animator() {}

	void Start() {}
	void Update() {}
};