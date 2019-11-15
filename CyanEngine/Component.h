#pragma once

class Component : public Object
{
private:
	bool isStarted{ false };

protected:
	Component() = default;
	Component(const Component&) = default;

public:
	GameObject* gameObject{ nullptr };

public:
	virtual ~Component();

	void UpdateComponent();

	virtual void Start() {}
	virtual void Update() {}
	virtual void Render() {}
	virtual void Destroy() {}

	virtual Component* Duplicate() { return new Component; };
};

