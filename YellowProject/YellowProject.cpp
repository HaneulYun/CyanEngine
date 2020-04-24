#include "pch.h"
#include "YellowProject.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
	CyanFW* cyanFW = new CyanFW(800, 600, L"YellowProject");

	SceneManager* sceneManager = SceneManager::Instance();
	sceneManager->scenes["menuScene"] = new MenuScene();
	sceneManager->scenes["mainScene"] = new SampleScene();
	sceneManager->scenes["materialScene"] = new MaterialScene();
	Scene::scene = sceneManager->scene = sceneManager->scenes["menuScene"];

	return CyanApp::Run(cyanFW, hInstance, nCmdShow);
}
