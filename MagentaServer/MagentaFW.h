#pragma once
#include "Time.h"

class MagentaFW
{
private:
	Scene* scene{ nullptr };

public:
	MagentaFW();
	~MagentaFW();

	bool OnCreate();
	void FrameAdvance();
	void OnDestroy();

};