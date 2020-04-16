#pragma once

class MeshFilter : public Component
{
public:
	Mesh* mesh{ nullptr };

public:
	MeshFilter() = default;
	MeshFilter(MeshFilter&) = default;
	~MeshFilter() = default;

	void Start() override {}
	void Update() override {}

	virtual Component* Duplicate() { return new MeshFilter; };
	virtual Component* Duplicate(Component* component) { return new MeshFilter(*(MeshFilter*)component); }

};