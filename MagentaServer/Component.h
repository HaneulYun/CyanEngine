#pragma once

class Component : public Object
{
private:
	bool isStarted{ false };

protected:
	Component() = default;
	Component(Component&) = default;

public:
	GameObject* gameObject{ nullptr };

public:
	virtual ~Component();

	void UpdateComponent();

	virtual void Start() {}
	virtual void Update() {}
	virtual void Destroy() {}

	virtual void OnTriggerEnter(GameObject* collision) {}
	virtual void OnTriggerStay(GameObject* collision) {}
	virtual void OnTriggerExit(GameObject* collision) {}
	virtual void OnCollisionEnter(GameObject* other) {}
	virtual void OnCollisionStay(GameObject* other) {}
	virtual void OnCollisionExit(GameObject* other) {}

	virtual Component* Duplicate(Component* component) { return new Component(*component); }
};

