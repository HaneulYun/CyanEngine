#pragma once

class SkinnedMeshRenderer : public MonoBehavior<SkinnedMeshRenderer, Renderer>
{
public:
	Mesh* mesh{ nullptr };
	GameObject* rootBone{ nullptr }; // unity is Transform
	std::vector<Transform*> bones;

private:
	friend class GameObject;
	friend class MonoBehavior<SkinnedMeshRenderer, Renderer>;
	SkinnedMeshRenderer() = default;
	SkinnedMeshRenderer(SkinnedMeshRenderer&) = default;

public:
	~SkinnedMeshRenderer() {}

	void Start();
};