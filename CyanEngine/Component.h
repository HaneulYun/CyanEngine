#pragma once

class Component : public Object
{
protected:
	Component();

public:
	GameObject* gameObject{ nullptr };

public:
	virtual ~Component();

	virtual void Start() {}
	virtual void Update() {}
	virtual void Render() {}
	virtual void Destroy() {}

	virtual Component* Duplicate() { return new Component; };
};

