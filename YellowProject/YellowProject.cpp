#include "pch.h"
#include "YellowProject.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
	CyanFW* cyanFW = new CyanFW(800, 600, L"YellowProject");

	SceneManager* sceneManager = SceneManager::Instance();
	sceneManager->scenes["MenuScene"] = new MenuScene();
	sceneManager->scenes["MainScene"] = new SampleScene();
	sceneManager->scenes["MaterialScene"] = new MaterialScene();
	Scene::scene = sceneManager->scene = sceneManager->scenes["MenuScene"];

	return CyanApp::Run(cyanFW, hInstance, nCmdShow);
}
