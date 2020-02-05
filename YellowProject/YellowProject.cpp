#include "pch.h"
#include "YellowProject.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	CyanFW cyanFW(1280, 720, L"YellowProject");
	cyanFW.OnSetScene(new SampleScene());
	return CyanApp::Run(&cyanFW, hInstance, nCmdShow);
}
