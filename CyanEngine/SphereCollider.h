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

	void Start(/*초기화 코드를 작성하세요.*/)
	{
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
	}

	bool Compare(Collider* _other);
};