#pragma once

class MeshFilter : public Component
{
public:
	Mesh* mesh{ nullptr };
	MeshGeometry* Geo{ nullptr };

	UINT IndexCount{ 0 };
	UINT StartIndexLocation{ 0 };
	int BaseVertexLocation{ 0 };

public:
	MeshFilter();
	MeshFilter(MeshFilter&) = default;
	~MeshFilter();

	void Start() override {}
	void Update() override {}

	virtual Component* Duplicate() { return new MeshFilter; };
	virtual Component* Duplicate(Component* component) { return new MeshFilter(*(MeshFilter*)component); }

};