#pragma once

class MeshFilter : public Component
{
public:
	Mesh* mesh{ nullptr };

public:
	MeshFilter();
	~MeshFilter();

	void Start() override {}
	void Update() override {}
	void Render() override {}
	void Destroy() override {}

	virtual Component* Duplicate() { return new MeshFilter; };
};