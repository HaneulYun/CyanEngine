#include "pch.h"
#include "YellowProject.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
	CyanFW* cyanFW = new CyanFW(800, 600, L"YellowProject");

	SceneManager* sceneManager = SceneManager::Instance();
	sceneManager->scenes["TerrainScene"] = new TerrainScene();
	sceneManager->scenes["MaterialScene"] = new MaterialScene();
	Scene::scene = sceneManager->scene = sceneManager->scenes["TerrainScene"];

	return CyanApp::Run(cyanFW, hInstance, nCmdShow);
}
