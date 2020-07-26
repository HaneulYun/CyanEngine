#include "pch.h"
#include "YellowProject.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
	CyanFW* cyanFW = new CyanFW(1280, 720, L"YellowProject");

	SceneManager* sceneManager = SceneManager::Instance();
	sceneManager->scenes["MenuScene"] = new MenuScene();
	sceneManager->scenes["TerrainScene"] = new TerrainScene();
	sceneManager->scenes["ParticleScene"] = new ParticleScene();
	sceneManager->scenes["CharacterScene"] = new CharacterScene();
	Scene::scene = sceneManager->scene = sceneManager->scenes["MenuScene"];

	return CyanApp::Run(cyanFW, hInstance, nCmdShow);
}
