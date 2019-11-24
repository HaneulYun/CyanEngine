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
		extents = { 14, 14, 14 };
	}
	
	void Update(/*업데이트 코드를 작성하세요.*/)
	{
	}

	bool Compare(BoxCollider* other);
};


//#pragma once
//#include "framework.h"
//
//class ScriptForm : public MonoBehavior<ScriptForm>
//{
//private /*이 영역에 private 변수를 선언하세요.*/:
//
//public  /*이 영역에 public 변수를 선언하세요.*/:
//
//private:
//	friend class GameObject;
//	friend class MonoBehavior<ScriptForm>;
//	ScriptForm() = default;
//	ScriptForm(ScriptForm&) = default;
//
//public:
//	~ScriptForm() {}
//
//	void Start(/*초기화 코드를 작성하세요.*/)
//	{
//	}
//
//	void Update(/*업데이트 코드를 작성하세요.*/)
//	{
//	}
//
//	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
//};

