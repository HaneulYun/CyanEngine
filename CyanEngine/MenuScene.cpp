#include "pch.h"
#include "MenuScene.h"

void MenuScene::BuildObjects()
{
	///*** Asset ***///
	//*** Texture ***//
	ASSET AddTexture("none", L"..\\YellowProject\\Textures\\none.dds");
	ASSET AddTexture("menuBackgroundTex", L"..\\YellowProject\\Textures\\menu\\background.dds");

	//*** Material ***//
	ASSET AddMaterial("none", ASSET TEXTURE("none"));
	ASSET AddMaterial("gray", ASSET TEXTURE("none"), 0, { 0.5, 0.5, 0.5, 0.5 });
	ASSET AddMaterial("menuBackgroundMat", ASSET TEXTURE("menuBackgroundTex"), 0, { 0.8, 0.8, 0.8, 1 });

	//*** Mesh ***//
	ASSET AddMesh("Image", Mesh::CreateQuad());
	ASSET AddMesh("Sphere", Mesh::CreateSphere());

	///*** Game Object ***///

	auto mainCamera = CreateEmpty();
	{
		camera = camera->main = mainCamera->AddComponent<Camera>();
	}

	auto background = CreateImage();
	{
		background->GetComponent<Renderer>()->materials[0] = ASSET MATERIAL("menuBackgroundMat");

		auto rectTransform = background->GetComponent<RectTransform>();
		rectTransform->anchorMin = { 0, 0 };
		rectTransform->anchorMax = { 1, 1 };

		auto text = background->AddComponent<Text>();
		text->text = L"두드리오\n\n\n";
		text->font = L"메이플스토리";
		text->fontSize = 120;
		text->color = { 1, 1, 1, 1 };
		text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
		text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;

		auto inputField = CreateImage();
		{
			auto rt = inputField->GetComponent<RectTransform>();
			rt->setAnchorAndPivot(0, 1);
			rt->setPosAndSize(10, -10, 150, 30);

			inputField->AddComponent<InputField>();
		}

		auto terrainSceneButton = CreateImage();
		{
			auto rt = terrainSceneButton->GetComponent<RectTransform>();
			rt->setAnchorAndPivot(0.5, 0.5);
			rt->setPosAndSize(-10, 20, 150, 30);

			terrainSceneButton->AddComponent<Button>()->AddEvent(
				[](void*) {
					SceneManager::LoadScene("TerrainScene");
				});
			{
				auto textobject = terrainSceneButton->AddChildUI();
				auto rectTransform = textobject->GetComponent<RectTransform>();
				rectTransform->anchorMin = { 0, 0 };
				rectTransform->anchorMax = { 1, 1 };

				Text* text = textobject->AddComponent<Text>();
				text->text = L"Terrain Scene";
				text->font = L"메이플스토리";
				text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
				text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			}
		}

		auto characterSceneButton = CreateImage();
		{
			auto rt = characterSceneButton->GetComponent<RectTransform>();
			rt->setAnchorAndPivot(0.5, 0.5);
			rt->setPosAndSize(-10, -60, 150, 30);

			characterSceneButton->AddComponent<Button>()->AddEvent(
				[](void*) {
					SceneManager::LoadScene("CharacterScene");
				});
			{
				auto textobject = characterSceneButton->AddChildUI();
				auto rectTransform = textobject->GetComponent<RectTransform>();
				rectTransform->anchorMin = { 0, 0 };
				rectTransform->anchorMax = { 1, 1 };

				Text* text = textobject->AddComponent<Text>();
				text->text = L"Character Scene";
				text->font = L"메이플스토리";
				text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
				text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			}
		}

		auto particleSceneButton = CreateImage();
		{
			auto rt = particleSceneButton->GetComponent<RectTransform>();
			rt->setAnchorAndPivot(0.5, 0.5);
			rt->setPosAndSize(-10, -140, 150, 30);

			particleSceneButton->AddComponent<Button>()->AddEvent(
				[](void*) {
					SceneManager::LoadScene("ParticleScene");
				});
			{
				auto textobject = particleSceneButton->AddChildUI();
				auto rectTransform = textobject->GetComponent<RectTransform>();
				rectTransform->anchorMin = { 0, 0 };
				rectTransform->anchorMax = { 1, 1 };

				Text* text = textobject->AddComponent<Text>();
				text->text = L"Particle Scene";
				text->font = L"메이플스토리";
				text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
				text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			}
		}
	}
}