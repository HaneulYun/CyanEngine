#include "pch.h"
#include "MagentaFW.h"

MagentaFW::MagentaFW()
{
}

MagentaFW::~MagentaFW()
{
}

bool MagentaFW::OnCreate()
{
	scene = new GameScene();
	if (scene)
		scene->Start();

	Time::Instance()->Reset();

	return true;
}

void MagentaFW::OnDestroy()
{
}

void MagentaFW::FrameAdvance()
{
	Time::Instance()->Tick();

	scene->Update();
}