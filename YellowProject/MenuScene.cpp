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
	ASSET AddMaterial("gray", ASSET TEXTURE("none"), 0, 0, 0, { 0.5, 0.5, 0.5, 0.5 });
	ASSET AddMaterial("menuBackgroundMat", ASSET TEXTURE("menuBackgroundTex"), 0, 0, 0, { 0.8, 0.8, 0.8, 1 });

	ASSET AddTexture("house01_D", L"Assets\\AdvancedVillagePack\\Textures\\T_Pack_04_D.dds");
	ASSET AddTexture("house01_N", L"Assets\\AdvancedVillagePack\\Textures\\T_Pack_04_N.dds");
	ASSET AddTexture("house01_E", L"Assets\\AdvancedVillagePack\\Textures\\T_Pack_04_E.dds");
	ASSET AddFbxForMesh("SM_House_Var01", "Assets\\AdvancedVillagePack\\Meshes\\SM_House_Var01.FBX");
	ASSET AddMaterial("house01", ASSET TEXTURE("house01_D"), ASSET TEXTURE("house01_N"), ASSET TEXTURE("house01_E"), 0, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.01f, 0.01f, 0.01f }, 0.9f);

	////*** AudioClip ***//
	ASSET AddAudioClip("testSound", "Assets\\FootstepSound\\Grass\\test.mp3");

	///*** Game Object ***///

	auto mainCamera = CreateEmpty();
	{
		camera = camera->main = mainCamera->AddComponent<Camera>();
		mainCamera->AddComponent<CameraController>();
	}

	auto soundBox = CreateEmpty();
	{
		auto audioSource = soundBox->AddComponent<AudioSource>();
		audioSource->clip = ASSET AUDIO_CLIP("testSound");
		audioSource->loop = true;
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
			rt->setPosAndSize(0, 20, 150, 30);

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
			rt->setPosAndSize(0, -60, 150, 30);

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
			rt->setPosAndSize(0, -140, 150, 30);

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

	auto testImage = CreateImage();
	{
		auto rt = testImage->GetComponent<RectTransform>();
		rt->setAnchorAndPivot(0, 1);
		rt->setPosAndSize(100, -300, 150, 150);
	}

	auto modelViewCameraObject = CreateEmpty();
	auto modelViewCamera = modelViewCameraObject->AddComponent<Camera>();

	auto gameObject = CreateEmpty();
	{
		Matrix4x4 projection = Matrix4x4::MatrixOrthographicOffCenterLH(-5, 5, -5, 5, -10, 10);

		Vector3 pos = Vector3(1, 1, -1).Normalize();
		Vector3 lookAt = Vector3(0);
		Vector3 up{ 0, 1, 0 };
		Matrix4x4 view = modelViewCamera->view = Matrix4x4::MatrixLookAtLH(pos, lookAt, up);

		gameObject->transform->Scale({ 1, 1, 1 });
		gameObject->transform->Rotate({ 1, 0, 0 }, -90);
		gameObject->transform->position = { 0, -2.5, 0 };
		gameObject->transform->localToWorldMatrix = gameObject->transform->localToWorldMatrix * view * projection;

		gameObject->AddComponent<MeshFilter>()->mesh = ASSET MESH("SM_House_Var01");
		gameObject->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("house01"));
		gameObject->AddComponent<Image>();
		gameObject->layer = (int)RenderLayer::OnUI;

		auto onUI = gameObject->AddComponent<OnUI>();
		onUI->leftTop.x = 100;
		onUI->leftTop.y = 300;
		onUI->rightBottom.x = 150;
		onUI->rightBottom.y = 150;
	}
}