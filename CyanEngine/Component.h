#pragma once

class Component : public Object
{
public:
	Component();
	~Component();

	virtual void OnStart() = 0;

	virtual void Update() = 0;
	virtual void Render() = 0;

	virtual void OnDestroy() = 0;
};

