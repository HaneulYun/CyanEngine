#pragma once

class Component : public Object
{
public:
	GameObject* gameObject{ nullptr };

public:
	Component();
	~Component();

	virtual void Start() = 0;

	virtual void Update() = 0;
	virtual void Render() = 0;

	virtual void Destroy() = 0;
};

