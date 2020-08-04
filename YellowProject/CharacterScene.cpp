#include "pch.h"
#include "CharacterScene.h"

void CharacterScene::BuildObjects()
{
	///*** Asset ***///
	//*** Texture ***//
	{
		ASSET AddTexture("none", L"Textures\\none.dds");
		ASSET AddTexture("polyArtTex", L"Textures\\PolyArtTex.dds");
		ASSET AddTexture("grass", L"Textures\\grass01.dds");
		ASSET AddTexture("ground", L"Textures\\ground.dds");
	}

	//*** Material ***//
	{
		ASSET AddMaterial("none", ASSET TEXTURE("none"));
		ASSET AddMaterial("PolyArt", ASSET TEXTURE("polyArtTex"), 0, 0, 0, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.01f, 0.01f, 0.01f }, 0.9f);
		ASSET AddMaterial("grass", ASSET TEXTURE("grass"), 0, 0, 0, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.01f, 0.01f, 0.01f }, 0.9f);
		ASSET AddMaterial("ground", ASSET TEXTURE("ground"), 0, 0, 0, { 0.48f, 0.64f, 0.2f, 1.0f }, { 0.01f, 0.01f, 0.01f }, 0.9f, Matrix4x4::MatrixScaling(200, 200, 200));
	}

	//*** Mesh ***//
	{
		ASSET AddFbxForAnimation("ApprenticeSK", "Models\\modelTest.fbx");
	}

	//*** AudioClip ***//
	{
		ASSET AddAudioClip("bgm", "Assets\\FootstepSound\\Grass\\test.mp3");
		ASSET AddAudioClip("footstep", "Assets\\FootstepSound\\Grass\\grass_1.WAV");
	}

	//*** Animation ***//
	{
		ASSET AddFbxForAnimation("Walk_BowAnim", "Models\\BowStance\\Walk_BowAnim.fbx");
		ASSET AddFbxForAnimation("WalkBack_BowAnim", "Models\\BowStance\\WalkBack_BowAnim.fbx");
		ASSET AddFbxForAnimation("WalkRight_BowAnim", "Models\\BowStance\\WalkRight_BowAnim.fbx");
		ASSET AddFbxForAnimation("WalkLeft_BowAnim", "Models\\BowStance\\WalkLeft_BowAnim.fbx");
		ASSET AddFbxForAnimation("Idle_BowAnim", "Models\\BowStance\\Idle_BowAnim.fbx");
	}

	//*** AnimatorController ***//
	AnimatorController* controller = new AnimatorController();
	{
		controller->AddParameterFloat("VelocityX");
		controller->AddParameterFloat("VelocityZ");

		controller->AddState("Idle", ASSET animationClips["Idle_BowAnim"].get());
		controller->AddState("Walk", ASSET animationClips["Walk_BowAnim"].get());
		controller->AddState("WalkBack", ASSET animationClips["WalkBack_BowAnim"].get());
		controller->AddState("WalkRight", ASSET animationClips["WalkRight_BowAnim"].get());
		controller->AddState("WalkLeft", ASSET animationClips["WalkLeft_BowAnim"].get());

		controller->AddTransition("Idle", "Walk", TransitionCondition::CreateFloat("VelocityZ", Greater, 0.3));
		controller->AddTransition("Idle", "WalkBack", TransitionCondition::CreateFloat("VelocityZ", Less, -0.3));
		controller->AddTransition("Walk", "Idle", TransitionCondition::CreateFloat("VelocityZ", Less, 0.3));
		controller->AddTransition("WalkBack", "Idle", TransitionCondition::CreateFloat("VelocityZ", Greater, -0.3));

		controller->AddTransition("Idle", "WalkLeft", TransitionCondition::CreateFloat("VelocityX", Greater, 0.3));
		controller->AddTransition("Idle", "WalkRight", TransitionCondition::CreateFloat("VelocityX", Less, -0.3));
		controller->AddTransition("WalkLeft", "Idle", TransitionCondition::CreateFloat("VelocityX", Less, 0.3));
		controller->AddTransition("WalkRight", "Idle", TransitionCondition::CreateFloat("VelocityX", Greater, -0.3));
	}

	///*** Game Object ***///

	auto soundBox = CreateEmpty();
	{
		soundBox->transform->position = { 512, 35, 512 };
		soundBox->transform->Scale({ 10, 10, 10 });
		soundBox->AddComponent<MeshFilter>()->mesh = ASSET MESH("Cube");
		soundBox->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("none"));
		auto audioSource = soundBox->AddComponent<AudioSource>();
		audioSource->clip = ASSET AUDIO_CLIP("bgm");
		audioSource->loop = true;
	}

	float TerrainSize = 1024;
	GameObject* terrain = CreateEmpty();
	auto terrainData = terrain->AddComponent<Terrain>();
	{
		{
			terrainData->terrainData.AlphamapTextureName = L"Texture\\f5o3_1024.raw";
			terrainData->terrainData.heightmapHeight = TerrainSize;
			terrainData->terrainData.heightmapWidth = TerrainSize;

			terrainData->terrainData.size = { TerrainSize, 255, TerrainSize };

			terrainData->Set();
		}
		terrain->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("ground"));
	}

	auto sim0 = CreateEmpty();
	{
		sim0->transform->position = { 0, 0, 0 };
	
		auto model = sim0->AddChild();
		{
			model->GetComponent<Transform>()->Rotate({ 1, 0, 0 }, -90);
			model->AddComponent<SkinnedMeshRenderer>()->mesh = ASSET MESH("ApprenticeSK");
			model->GetComponent<SkinnedMeshRenderer>()->materials.push_back(ASSET MATERIAL("PolyArt"));
		}
	
		auto ccontroller = sim0->AddComponent<CharacterController>();
		ccontroller->isPlayer = false;
		ccontroller->terrainData = &terrainData->terrainData;
	
		auto anim = sim0->AddComponent<Animator>();
		anim->controller = controller;
		anim->state = &controller->states["Idle"];
		anim->TimePos = 0;

		auto uiPos = sim0->AddChild();
		{
			uiPos->transform->position = { 0, 1.8, 0 };
			auto uiBox = uiPos->AddChildUI();
			{
				auto rt = uiBox->GetComponent<RectTransform>();
				rt->renderMode = RectTransform::WorldSpace;

				auto text = uiBox->AddComponent<Text>();
				text->text = L"Zenny";
				text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
				text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			}
		}
	}

	auto player = CreateEmpty();
	{
		player->transform->position = { 0, 0, -2 };

		auto model = player->AddChild();
		{
			model->GetComponent<Transform>()->Rotate({ 1, 0, 0 }, -90);
			auto skinnedMeshRenderer = model->AddComponent<SkinnedMeshRenderer>();
			skinnedMeshRenderer->mesh = ASSET MESH("ApprenticeSK");
			skinnedMeshRenderer->materials.push_back(ASSET MATERIAL("PolyArt"));
			skinnedMeshRenderer->Root();
		}
		auto audioSource = model->AddComponent<AudioSource>();
		audioSource->spatial = true;
		audioSource->clip = ASSET AUDIO_CLIP("footstep");

		auto ccontroller = player->AddComponent<CharacterController>();
		ccontroller->terrainData = &terrainData->terrainData;
		ccontroller->audioSource = audioSource;

		auto anim = player->AddComponent<Animator>();
		anim->controller = controller;
		anim->state = &controller->states["Idle"];
		anim->TimePos = 0.5;

		auto mainCamera = player->AddChild();
		{
			mainCamera->transform->position = { 0, 2, -3 };
			camera = camera->main = mainCamera->AddComponent<Camera>();
			mainCamera->AddComponent<AudioListener>();
		}

		auto uiPos = player->AddChild();
		{
			uiPos->transform->position = { 0, 1.8, 0 };
			auto uiBox = uiPos->AddChildUI();
			{
				auto rt = uiBox->GetComponent<RectTransform>();
				rt->renderMode = RectTransform::WorldSpace;

				auto text = uiBox->AddComponent<Text>();
				text->text = L"NiL";
				text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
				text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			}
		}
	}

	auto sim = CreateEmpty();
	{
		sim->transform->position = { 2, 0, 0 };
	
		auto model = sim->AddChild();
		{
			model->GetComponent<Transform>()->Rotate({ 1, 0, 0 }, -90);
			model->AddComponent<SkinnedMeshRenderer>()->mesh = ASSET MESH("ApprenticeSK");
			model->GetComponent<SkinnedMeshRenderer>()->materials.push_back(ASSET MATERIAL("PolyArt"));
		}
	
		auto ccontroller = sim->AddComponent<CharacterController>();
		ccontroller->isPlayer = false;
		ccontroller->terrainData = &terrainData->terrainData;
	
		auto anim = sim->AddComponent<Animator>();
		anim->controller = controller;
		anim->state = &controller->states["Idle"];
		anim->TimePos = 1;

		auto uiPos = sim->AddChild();
		{
			uiPos->transform->position = { 0, 1.8, 0 };
			auto uiBox = uiPos->AddChildUI();
			{
				auto rt = uiBox->GetComponent<RectTransform>();
				rt->renderMode = RectTransform::WorldSpace;

				auto text = uiBox->AddComponent<Text>();
				text->text = L"Yenny";
				text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
				text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			}
		}
	}

	auto directionalLight = CreateEmpty();
	{
		directionalLight->transform->Rotate({ 1, 0, 0 }, 90);
		auto light = directionalLight->AddComponent<Light>();
		light->Strength = { 0.9f, 0.8f, 0.7f };
		light->shadowType = Light::Shadows;

		directionalLight->AddComponent<RotatingBehavior>()->setAxisAndSpeed({ 0, 1, 0 }, 360 * 0.05);

		environment.sunSources = light;
	}

	{
		auto skybox = CreateEmpty();
		skybox->transform->Scale({ 5000.0f, 5000.0f, 5000.0f });
		skybox->AddComponent<MeshFilter>()->mesh = ASSET MESH("Sphere");
		skybox->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("none"));
		skybox->layer = (int)RenderLayer::Sky;
	}

	auto terrainBottom = CreateEmpty();
	{
		terrainBottom->transform->Scale({ 1024, 1, 1024 });
		terrainBottom->transform->Rotate({ 0, 0, 1 }, 180);
		terrainBottom->transform->position = { 512, 0, 512 };
		terrainBottom->AddComponent<MeshFilter>()->mesh = ASSET MESH("Plane");
		terrainBottom->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("none"));
	}

	auto menuSceneButton = CreateImage();
	{
		auto rectTransform = menuSceneButton->GetComponent<RectTransform>();
		rectTransform->setAnchorAndPivot(0, 1);
		rectTransform->setPosAndSize(10, -10, 150, 30);

		menuSceneButton->AddComponent<Button>()->AddEvent(
			[](void*) {
				SceneManager::LoadScene("MenuScene");
			});
		{
			auto textobject = menuSceneButton->AddChildUI();
			auto rectTransform = textobject->GetComponent<RectTransform>();
			rectTransform->anchorMin = { 0, 0 };
			rectTransform->anchorMax = { 1, 1 };

			Text* text = textobject->AddComponent<Text>();
			text->text = L"Menu Scene";
			text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
			text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
		}
	}
}