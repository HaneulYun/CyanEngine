#include "pch.h"
#include "YellowProject.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
	CyanFW* cyanFW = new CyanFW(800, 600, L"YellowProject");

	SceneManager* sceneManager = SceneManager::Instance();
	sceneManager->scenes["MenuScene"] = new MenuScene();
	sceneManager->scenes["TerrainScene"] = new TerrainScene();
	sceneManager->scenes["MaterialScene"] = new MaterialScene();
	sceneManager->scenes["ParticleScene"] = new ParticleScene();
	Scene::scene = sceneManager->scene = sceneManager->scenes["ParticleScene"];

	return CyanApp::Run(cyanFW, hInstance, nCmdShow);
}
