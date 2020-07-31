#pragma once

class Component : public Object
{
private:
	bool isDirty{ true };

public:
	bool enabled{ true };

protected:
	Component() = default;
	Component(Component& component) = default;

public:
	GameObject* gameObject{ nullptr };

public:
	virtual ~Component()
	{
		OnDestroy();
	}

	//void UpdateComponent();

	virtual void Start() {}
	virtual void Update() {}

	virtual void OnTriggerEnter(GameObject* collision) {}
	virtual void OnTriggerStay(GameObject* collision) {}
	virtual void OnTriggerExit(GameObject* collision) {}
	virtual void OnCollisionEnter(GameObject* other) {}
	virtual void OnCollisionStay(GameObject* other) {}
	virtual void OnCollisionExit(GameObject* other) {}

	virtual void OnDestroy() {}

	virtual Component* Duplicate(Component* component) { return new Component(*component); }
};

