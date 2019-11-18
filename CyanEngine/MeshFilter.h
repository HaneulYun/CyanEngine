#pragma once

class MeshFilter : public Component
{
public:
	Mesh* mesh{ nullptr };

public:
	MeshFilter();
	MeshFilter(MeshFilter&) = default;
	~MeshFilter();

	void Start() override {}
	void Update() override {}
	void Render() override {}

	virtual Component* Duplicate() { return new MeshFilter; };
	virtual Component* Duplicate(Component* component) { return new MeshFilter(*(MeshFilter*)component); }

};