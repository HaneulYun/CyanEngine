#pragma once

class Component : public Object
{
protected:
	Component();

public:
	GameObject* gameObject{ nullptr };

public:
	virtual ~Component();

	virtual void Start() = 0;

	virtual void Update() = 0;
	virtual void Render() = 0;

	virtual void Destroy() = 0;
};

