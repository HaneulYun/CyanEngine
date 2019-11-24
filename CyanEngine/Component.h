#pragma once

class Collision;
class Collider;

class Component : public Object
{
private:
	bool isStarted{ false };

protected:
	Component() = default;
	Component(Component& component) = default;

public:
	GameObject* gameObject{ nullptr };

public:
	virtual ~Component() {}

	void UpdateComponent();

	virtual void Start() {}
	virtual void Update() {}

	virtual void OnTriggerEnter(Collision* collision) {}
	virtual void OnTriggerStay(Collision* collision) {}
	virtual void OnTriggerExit(Collision* collision) {}
	virtual void OnCollisionEnter(Collider* other) {}
	virtual void OnCollisionStay(Collider* other) {}
	virtual void OnCollisionExit(Collider* other) {}

	virtual Component* Duplicate(Component* component) { return new Component(*component); }
};

