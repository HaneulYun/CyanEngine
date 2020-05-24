#include "pch.h"
#include "YellowProject.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
	CyanFW* cyanFW = new CyanFW(800, 800, L"YellowProject");

	SceneManager* sceneManager = SceneManager::Instance();
	sceneManager->scenes["GameScene"] = new GameScene();
	Scene::scene = sceneManager->scene = sceneManager->scenes["GameScene"];

	return CyanApp::Run(cyanFW, hInstance, nCmdShow);
}
