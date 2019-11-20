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
	virtual void Render() {}

	virtual Component* Duplicate(Component* component) { return new Component(*component); }
};

