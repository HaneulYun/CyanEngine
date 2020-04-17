#pragma once

class SkinnedMeshRenderer : public Renderer
{
public:
	Mesh* mesh{ nullptr };

private:
	friend class GameObject;

public:
	SkinnedMeshRenderer();
	SkinnedMeshRenderer(SkinnedMeshRenderer& component)
	{
	}
	~SkinnedMeshRenderer();

	void Start() override;
	void Update() override;

	virtual Component* Duplicate() { return new SkinnedMeshRenderer; };
	virtual Component* Duplicate(Component* component) { return
		new SkinnedMeshRenderer(*(SkinnedMeshRenderer*)component); }
};