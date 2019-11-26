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

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
	}
	
	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	bool Compare(Collider* _other);
};