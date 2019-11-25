#pragma once

class SphereCollider : public Collider
{
public:
	union
	{
		BoundingSphere boundingSphere;
		struct
		{
			Vector3 center;
			float radius;
		};
	};

private:
	friend class GameObject;
	//friend class MonoBehavior<ScriptForm>;
	SphereCollider() {};
	SphereCollider(SphereCollider&) = default;

	virtual Component* Duplicate() { return new SphereCollider; };
	virtual Component* Duplicate(Component* component) { return new SphereCollider(*(SphereCollider*)component); }


public:
	~SphereCollider() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	bool Compare(Collider* _other);
};