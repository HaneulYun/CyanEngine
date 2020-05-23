#include "pch.h"
#include "MenuScene.h"

void MenuScene::BuildObjects()
{
	///*** Asset ***///
	//*** Texture ***//
	ASSET AddTexture("none", L"Textures\\none.dds");
	ASSET AddTexture("menuBackgroundTex", L"Textures\\menu\\background.dds");

	//*** Material ***//
	ASSET AddMaterial("none", ASSET TEXTURE("none"));
	ASSET AddMaterial("gray", ASSET TEXTURE("none"), -1, { 0.5, 0.5, 0.5, 0.5 });
	ASSET AddMaterial("menuBackgroundMat", ASSET TEXTURE("menuBackgroundTex"), -1, { 0.8, 0.8, 0.8, 1 });

	//*** Mesh ***//
	ASSET AddMesh("Image", Mesh::CreateQuad());
	ASSET AddMesh("Sphere", Mesh::CreateSphere());

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
		text->text = L"�ε帮��\n\n\n";
		text->font = L"�����ý��丮";
		text->fontSize = 120;
		text->color = { 1, 1, 1, 1 };
		text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
		text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
		textObjects.push_back(background);

		auto sampleSceneButton = CreateImage();
		{
			auto rectTransform = sampleSceneButton->GetComponent<RectTransform>();
			rectTransform->anchorMin = { 0.5, 0.5 };
			rectTransform->anchorMax = { 0.5, 0.5 };
			rectTransform->pivot = { 0.5, 0.5 };
			rectTransform->posX = -10;
			rectTransform->posY = 20;
			rectTransform->width = 150;
			rectTransform->height = 30;

			sampleSceneButton->AddComponent<Button>()->AddEvent(
				[](void*) {
					Debug::Log("�̰� �ǳ�;;\n");
					SceneManager::LoadScene("SampleScene");
				});
			{
				auto textobject = sampleSceneButton->AddChildUI();
				auto rectTransform = textobject->GetComponent<RectTransform>();
				rectTransform->anchorMin = { 0, 0 };
				rectTransform->anchorMax = { 1, 1 };

				Text* text = textobject->AddComponent<Text>();
				text->text = L"Sample Scene";
				text->font = L"�����ý��丮";
				text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
				text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
				textObjects.push_back(textobject);
			}
			sampleSceneButton->GetComponent<Renderer>()->materials[0] = ASSET MATERIAL("none");
		}
	}
}