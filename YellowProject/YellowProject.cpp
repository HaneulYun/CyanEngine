#include "pch.h"
#include "YellowProject.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	CyanFW cyanFW;
	return CyanApp::Run(&cyanFW, hInstance, nCmdShow);
}
