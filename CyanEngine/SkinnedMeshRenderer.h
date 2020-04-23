#pragma once

class SkinnedMeshRenderer : public MonoBehavior<SkinnedMeshRenderer, Renderer>
{
public:
	Mesh* mesh{ nullptr };

private:
	friend class GameObject;
	friend class MonoBehavior<SkinnedMeshRenderer, Renderer>;
	SkinnedMeshRenderer() = default;
	SkinnedMeshRenderer(SkinnedMeshRenderer&) = default;

public:
	~SkinnedMeshRenderer() {}
};