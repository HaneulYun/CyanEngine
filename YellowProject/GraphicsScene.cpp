#include "pch.h"
#include "GraphicsScene.h"

void GraphicsScene::BuildObjects()
{
	///*** Asset ***///


	///*** Game Object ***///

	auto mainCamera = CreateEmpty();
	{
		camera = camera->main = mainCamera->AddComponent<Camera>();
		mainCamera->AddComponent<CameraController>();
	}

	auto background = CreateImage();
	{
		background->GetComponent<Renderer>()->materials[0] = ASSET MATERIAL("menuBackgroundMat");

		auto rectTransform = background->GetComponent<RectTransform>();
		rectTransform->anchorMin = { 0, 0 };
		rectTransform->anchorMax = { 1, 1 };

		auto text = background->AddComponent<Text>();
		{
			text->text = L"Graphics";
			text->fontSize = 120;
			textObjects.push_back(background);
		}
	}
}