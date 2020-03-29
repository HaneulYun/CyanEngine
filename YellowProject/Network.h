#pragma once
#include "framework.h"

class Network : public MonoBehavior<Network>
{
private:

public:
	char* serverIP;

private:
	friend class GameObject;
	friend class MonoBehavior<Network>;
	Network() = default;
	Network(Network&) = default;

public:
	~Network() {}

	void Start()
	{
		
	}

	void Update()
	{
		
	}

};
