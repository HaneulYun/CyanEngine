#include "pch.h"
#include "YellowProject.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
	CyanFW* cyanFW = new CyanFW(1280, 720, L"YellowProject");
	cyanFW->OnSetScene(new SampleScene());
	return CyanApp::Run(cyanFW, hInstance, nCmdShow);
}
