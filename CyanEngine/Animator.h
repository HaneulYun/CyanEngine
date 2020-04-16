#pragma once

class SkinnedModelInstance;

class Animator : public MonoBehavior<Animator>
{
public:
	SkinnedModelInstance* SkinnedModelInst = nullptr;
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