#include "pch.h"
#include "MenuScene.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
	int start{};

	CyanFW* cyanFW = new CyanFW(1280, 720, L"YellowProject");

	SceneManager* sceneManager = SceneManager::Instance();
	sceneManager->scenes["MenuScene"] = new MenuScene();
	Scene::scene = sceneManager->scene = sceneManager->scenes["MenuScene"];

	CyanApp::Run(cyanFW, hInstance, nCmdShow);

	return 0;
}
