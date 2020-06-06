#include "pch.h"
#include "YellowProject.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
	CyanFW* cyanFW = new CyanFW(1000, 1000, L"YellowProject");

	SceneManager* sceneManager = SceneManager::Instance();
	sceneManager->scenes["GameScene"] = new GameScene();
	Scene::scene = sceneManager->scene = sceneManager->scenes["GameScene"];

	return CyanApp::Run(cyanFW, hInstance, nCmdShow);
}
