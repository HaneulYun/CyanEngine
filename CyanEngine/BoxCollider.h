#pragma once

class BoxCollider : public Collider
{
public:
	union
	{
		BoundingBox boundingBox;
		struct
		{
			Vector3 center;
			Vector3 extents;
		};
	};
	Vector3 size{};

private:
	friend class GameObject;
	//friend class MonoBehavior<ScriptForm>;
	BoxCollider() {};
	BoxCollider(BoxCollider&) = default;

	virtual Component* Duplicate() { return new BoxCollider; };
	virtual Component* Duplicate(Component* component) { return new BoxCollider(*(BoxCollider*)component); }


public:
	~BoxCollider() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
	}
	
	void Update(/*업데이트 코드를 작성하세요.*/)
	{
	}

	bool Compare(Collider* _other);
};