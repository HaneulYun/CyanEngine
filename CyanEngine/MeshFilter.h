#pragma once

class MeshFilter : public MonoBehavior<MeshFilter>
{
public:
	Mesh* mesh{ nullptr };

private:
	friend class GameObject;
	friend class MonoBehavior<MeshFilter>;
	MeshFilter() = default;
	MeshFilter(MeshFilter&) = default;

public:
	~MeshFilter() {}

	void Start();
};