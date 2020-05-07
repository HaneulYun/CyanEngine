#include "pch.h"
#include "TerrainScene.h"

BuildManager* BuildManager::buildManager{ nullptr };
ButtonManager* ButtonManager::buttonManager{ nullptr };

void TerrainScene::BuildObjects()
{
	///*** Asset ***///
	//*** Texture ***//
	{
		ASSET AddTexture("none", L"Textures\\none.dds");
		ASSET AddTexture("polyArtTex", L"Textures\\PolyArtTex.dds");
		ASSET AddTexture("bricksTex", L"Textures\\bricks2.dds");
		ASSET AddTexture("stoneTex", L"Textures\\stone.dds");
		ASSET AddTexture("tileTex", L"Textures\\tile.dds");
		ASSET AddTexture("tree", L"Textures\\tree01S.dds");
		ASSET AddTexture("grass", L"Textures\\grass01.dds");
	}

	//*** Material ***//
	{
		ASSET AddMaterial("none",		ASSET TEXTURE("none"));
		ASSET AddMaterial("PolyArt",	ASSET TEXTURE("polyArtTex"), -1, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.01f, 0.01f, 0.01f }, 0.9f);
		ASSET AddMaterial("bricksMat",	ASSET TEXTURE("bricksTex"), -1, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.02f, 0.02f, 0.02f }, 0.1f);
		ASSET AddMaterial("stoneMat",	ASSET TEXTURE("none"), -1, { 0.0f, 0.0f, 0.1f, 1.0f }, { 0.98f, 0.97f, 0.95f }, 0.1f);
		ASSET AddMaterial("tile0",		ASSET TEXTURE("tileTex"), -1, { 0.9f, 0.9f, 0.9f, 1.0f }, { 0.02f, 0.02f, 0.02f }, 0.1f, Matrix4x4::MatrixScaling(8, 8, 1));
		ASSET AddMaterial("tree0",		ASSET TEXTURE("tree"), -1, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.01f, 0.01f, 0.01f }, 0.1f);
		ASSET AddMaterial("grass",		ASSET TEXTURE("grass"), -1, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.01f, 0.01f, 0.01f }, 0.1f);
		for (int i = 0; i < 6; ++i)
			AssetManager::Instance()->AddMaterial("material_" + std::to_string(i), 0, 0, RANDOM_COLOR, { 0.98f, 0.97f, 0.95f }, 0.0f);
	}

	//*** Mesh ***//
	{
		//ASSET AddMesh("Image", Mesh::CreateQuad());
		ASSET AddMesh("Cube", Mesh::CreateCube());
		//ASSET AddMesh("Plane", Mesh::CreatePlane());
		//ASSET AddMesh("Sphere", Mesh::CreateSphere());
		ASSET AddMesh("Cylinder", Mesh::CreateCylinder());
		ASSET AddFbxForAnimation("ApprenticeSK", "Models\\modelTest.fbx");
	}

	CHeightMapImage* m_pHeightMapImage = new CHeightMapImage(L"Texture\\heightMap.raw", 257, 257, { 1.0f, 0.1f, 1.0f });
	CHeightMapGridMesh* gridMesh = new CHeightMapGridMesh(0, 0, 257, 257, { 1, 1, 1 }, { 1, 1, 0, 1 }, m_pHeightMapImage);

	//*** Animation ***//
	{
		AssetManager::Instance()->AddFbxForAnimation("Walk_BowAnim", "Models\\BowStance\\Walk_BowAnim.fbx");
		AssetManager::Instance()->AddFbxForAnimation("WalkBack_BowAnim", "Models\\BowStance\\WalkBack_BowAnim.fbx");
		AssetManager::Instance()->AddFbxForAnimation("WalkRight_BowAnim", "Models\\BowStance\\WalkRight_BowAnim.fbx");
		AssetManager::Instance()->AddFbxForAnimation("WalkLeft_BowAnim", "Models\\BowStance\\WalkLeft_BowAnim.fbx");
		AssetManager::Instance()->AddFbxForAnimation("Idle_BowAnim", "Models\\BowStance\\Idle_BowAnim.fbx");
	}

	AnimatorController* controller = new AnimatorController();
	//*** AnimatorController ***//
	{
		controller->AddParameterFloat("Speed");
		controller->AddParameterFloat("HoriSpeed");

		controller->AddState("Idle", AssetManager::Instance()->animationClips["Idle_BowAnim"].get());
		controller->AddState("Walk", AssetManager::Instance()->animationClips["Walk_BowAnim"].get());
		controller->AddState("WalkBack", AssetManager::Instance()->animationClips["WalkBack_BowAnim"].get());
		controller->AddState("WalkRight", AssetManager::Instance()->animationClips["WalkRight_BowAnim"].get());
		controller->AddState("WalkLeft", AssetManager::Instance()->animationClips["WalkLeft_BowAnim"].get());

		controller->AddTransition("Idle", "Walk", TransitionCondition::CreateFloat("Speed", Greater, 0.1));
		controller->AddTransition("Idle", "WalkBack", TransitionCondition::CreateFloat("Speed", Less, -0.1));
		controller->AddTransition("Walk", "Idle", TransitionCondition::CreateFloat("Speed", Less, 0.1));
		controller->AddTransition("WalkBack", "Idle", TransitionCondition::CreateFloat("Speed", Greater, -0.1));

		controller->AddTransition("Idle", "WalkLeft", TransitionCondition::CreateFloat("HoriSpeed", Greater, 0.1));
		controller->AddTransition("Idle", "WalkRight", TransitionCondition::CreateFloat("HoriSpeed", Less, -0.1));
		controller->AddTransition("WalkLeft", "Idle", TransitionCondition::CreateFloat("HoriSpeed", Less, 0.1));
		controller->AddTransition("WalkRight", "Idle", TransitionCondition::CreateFloat("HoriSpeed", Greater, -0.1));
	}

	///*** Game Object ***///

	GameObject* mainCamera = CreateEmpty();
	{
		camera = camera->main = mainCamera->AddComponent<Camera>();
		mainCamera->AddComponent<CameraController>();
	}

	{
		auto ritem = CreateEmpty();
		ritem->GetComponent<Transform>()->Scale({ 5000.0f, 5000.0f, 5000.0f });
		auto mesh = ritem->AddComponent<MeshFilter>()->mesh = ASSET MESH("Sphere");
		auto renderer = ritem->AddComponent<Renderer>();
		for (auto& sm : mesh->DrawArgs)
			renderer->materials.push_back(ASSET MATERIAL("none"));
		ritem->layer = (int)RenderLayer::Sky;
	}

	std::string name[9]{
		"Attack01_BowAnim",
		"Attack01Maintain_BowAnim",
		"Attack01RepeatFire_BowAnim",
		"Attack01Start_BowAnim",
		"Attack02Maintain_BowAnim",
		"Attack02RepeatFire_BowAnim",
		"Attack02Start_BowAnim",
		"DashBackward_BowAnim",
		"DashForward_BowAnim",
	};

	int i = 0;
	int count = 1;
	float interval = 7.0f;
	for (int x = -count; x <= count; ++x)
		for (int z = -count; z <= count; ++z)
		{
			auto ritem = CreateEmpty();
			ritem->GetComponent<Transform>()->Scale({ 0.02, 0.02, 0.02 });
			ritem->GetComponent<Transform>()->Rotate({ 1, 0, 0 }, -90);
			ritem->GetComponent<Transform>()->position = { interval * x, 0.0f, interval * z };
			auto mesh = ritem->AddComponent<SkinnedMeshRenderer>()->mesh = ASSET MESH("ApprenticeSK");
			auto renderer = ritem->GetComponent<SkinnedMeshRenderer>();
			for (auto& sm : mesh->DrawArgs)
				renderer->materials.push_back(ASSET MATERIAL("PolyArt"));

			auto anim = ritem->AddComponent<Animator>();
			anim->controller = controller;
			anim->state = &controller->states["Idle"];
			anim->TimePos = Random::Range(0.0f, anim->controller->GetClipEndTime(anim->state));

			auto ref = ritem->AddComponent<CharacterController>();

			if (!x && !z)
			{
				ref->isPlayer = true;
			}

			ritem->layer = (int)RenderLayer::SkinnedOpaque;
		}

	//auto guestPlayer = CreateEmpty();
	//{
	//	auto model = guestPlayer->AddChild();
	//	{
	//		model->GetComponent<Transform>()->Scale({ 0.02, 0.02, 0.02 });
	//		model->GetComponent<Transform>()->Rotate({ 1, 0, 0 }, -90);
	//		auto mesh = model->AddComponent<SkinnedMeshRenderer>()->mesh = geometries["ApprenticeSK"].get();
	//		auto renderer = model->GetComponent<SkinnedMeshRenderer>();
	//		for (auto& sm : mesh->DrawArgs)
	//			renderer->materials.push_back(1);
	//
	//		auto animator = model->AddComponent<Animator>();
	//		animator->controller = controller;
	//		animator->state = &controller->states["Idle"];
	//		animator->TimePos = 0;
	//
	//		//guestPlayer->AddComponent<GuestController>()->animator = animator;
	//	}
	//	//auto cameraOffset = guestPlayer->AddChild();
	//	//{
	//	//	cameraOffset->transform->position = { 0, 3, -6 };
	//	//	camera = camera->main = cameraOffset->AddComponent<Camera>();
	//	//	cameraOffset->AddComponent<CameraController>();
	//	//}
	//}

	GameObject* prefab;
	int xObjects = 0, yObjects = 0, zObjects = 0;
	for (int x = -xObjects; x <= xObjects; x++)
		for (int y = -yObjects; y <= yObjects; y++)
			for (int z = -zObjects; z <= zObjects; z++)
			{
				auto ritem = CreateEmpty();
				prefab = ritem;
				ritem->GetComponent<Transform>()->Scale({ 5, 5, 5 });
				ritem->GetComponent<Transform>()->position = { 20.0f * x, 20.0f * y, 20.0f * z };
				auto mesh = ritem->AddComponent<MeshFilter>()->mesh = ASSET MESH("Cube");
				ritem->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("material_0"));

				ritem->AddComponent<RotatingBehavior>()->speedRotating = Random::Range(-10.0f, 10.0f) * 2;
			}

	GameObject* grid = CreateEmpty();
	{
		grid->GetComponent<Transform>()->position -= {128, 10, 128};
		auto mesh = grid->AddComponent<MeshFilter>()->mesh = gridMesh;
		grid->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("PolyArt"));
	}

	{
		GameObject* manager = CreateEmpty();
		BuildManager* buildManager = manager->AddComponent<BuildManager>();
		buildManager->terrain = grid;
		buildManager->heightMap = m_pHeightMapImage;
		buildManager->terrainMesh = gridMesh;
		//bm->SelectModel(geometries["Cube"].get(), 8, 5);
		BuildManager::buildManager = buildManager;
		ButtonManager* buttonManager = manager->AddComponent<ButtonManager>();
		ButtonManager::buttonManager = buttonManager;
	}

	// billboard points
	struct TreeSpriteVertex
	{
		XMFLOAT3 Pos;
		XMFLOAT2 Size;
		XMFLOAT3 look;
	};
	std::vector<TreeSpriteVertex> vertices;
	float sizex = 2, sizey = 2;
	const int width = 256, length = 256;
	vertices.reserve(width * length);
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < length; ++j)
		{
			TreeSpriteVertex v;
			v.Pos = XMFLOAT3(i, gridMesh->OnGetHeight(i, j, m_pHeightMapImage) + sizey / 2, j);
			v.Size = XMFLOAT2(sizex, sizey);
			v.look = XMFLOAT3(MathHelper::RandF(0.0f, 1.0f), 0.0f, MathHelper::RandF(0.0f, 1.0f));
			vertices.push_back(v);
		}
	}

	auto geo = std::make_unique<Mesh>();
	const UINT vbByteSize = (UINT)vertices.size() * sizeof(TreeSpriteVertex);

	geo->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU);
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	auto device = Graphics::Instance()->device;
	auto commandList = Graphics::Instance()->commandList;

	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(), commandList.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->VertexByteStride = sizeof(TreeSpriteVertex);
	geo->VertexBufferByteSize = vbByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = vertices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs["submesh"] = submesh;
	ASSET AddMesh("Grass", std::move(geo));
	{
		GameObject* billboards = CreateEmpty();
		billboards->GetComponent<Transform>()->position -= {128, 10, 128};
		auto mesh = billboards->AddComponent<MeshFilter>()->mesh = ASSET MESH("Grass");
		billboards->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("grass"));
		billboards->layer = (int)RenderLayer::Grass;
	}

	auto menuSceneButton = CreateImage();
	{
		auto rectTransform = menuSceneButton->GetComponent<RectTransform>();
		rectTransform->anchorMin = { 0, 1 };
		rectTransform->anchorMax = { 0, 1 };
		rectTransform->pivot = { 0, 1 };
		rectTransform->posX = 10;
		rectTransform->posY = -10;
		rectTransform->width = 150;
		rectTransform->height = 30;

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
			textObjects.push_back(textobject);
		}
	}
	for (int i = 0; i < 5; ++i)
	{
		GameObject* leftCylRItem = CreateEmpty();
		leftCylRItem->GetComponent<Transform>()->position = Vector3(-5.0f, 1.5f, -10.0f + i * 5.0f);
		auto mesh = leftCylRItem->AddComponent<MeshFilter>()->mesh = ASSET MESH("Cylinder");
		leftCylRItem->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("bricksMat"));

		GameObject* rightCylRItem = CreateEmpty();
		rightCylRItem->GetComponent<Transform>()->position = Vector3(5.0f, 1.5f, -10.0f + i * 5.0f);
		mesh = rightCylRItem->AddComponent<MeshFilter>()->mesh = ASSET MESH("Cylinder");
		rightCylRItem->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("bricksMat"));
	}

	// Build Button
	auto BSButton00 = CreateImage();
	ButtonManager::buttonManager->buttons.push_back(std::make_pair(BSButton00, false));
	{
		auto rectTransform = BSButton00->GetComponent<RectTransform>();
		rectTransform->anchorMin = { 0.5, 0 };
		rectTransform->anchorMax = { 0.5, 0 };
		rectTransform->pivot = { 0.5, 0 };
		rectTransform->posX = -70;
		rectTransform->posY = 70;
		rectTransform->width = 50;
		rectTransform->height = 50;

		BSButton00->AddComponent<Button>()->AddEvent(
			[](void*) {
				BuildManager::buildManager->SelectModel(ASSET MESH("Sphere"), ASSET MATERIAL("bricksMat"), 1);
			});
		{
			auto textobject = BSButton00->AddChildUI();
			auto rectTransform = textobject->GetComponent<RectTransform>();
			rectTransform->anchorMin = { 0, 0 };
			rectTransform->anchorMax = { 1, 1 };

			Text* text = textobject->AddComponent<Text>();
			text->text = L"건물1";
			text->fontSize = 10;
			text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
			text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			textObjects.push_back(textobject);
		}
		BSButton00->SetActive(false);
	}

	auto BuildingSelectButton01 = CreateImage();
	{
		auto rectTransform = BuildingSelectButton01->GetComponent<RectTransform>();
		rectTransform->anchorMin = { 0.5, 0 };
		rectTransform->anchorMax = { 0.5, 0 };
		rectTransform->pivot = { 0.5, 0 };
		rectTransform->posX = -70;
		rectTransform->posY = 10;
		rectTransform->width = 50;
		rectTransform->height = 50;

		BuildingSelectButton01->AddComponent<Button>()->AddEvent(
			[](void*) {
				ButtonManager::buttonManager->SelectButton(0);
			});
		{
			auto textobject = BuildingSelectButton01->AddChildUI();
			auto rectTransform = textobject->GetComponent<RectTransform>();
			rectTransform->anchorMin = { 0, 0 };
			rectTransform->anchorMax = { 1, 1 };

			Text* text = textobject->AddComponent<Text>();
			text->text = L"랜드마크";
			text->fontSize = 10;
			text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
			text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			textObjects.push_back(textobject);
		}
	}

	auto BuildingSelectButton02 = CreateImage();
	{
		auto rectTransform = BuildingSelectButton02->GetComponent<RectTransform>();
		rectTransform->anchorMin = { 0.5, 0 };
		rectTransform->anchorMax = { 0.5, 0 };
		rectTransform->pivot = { 0.5, 0 };
		rectTransform->posX = -10;
		rectTransform->posY = 10;
		rectTransform->width = 50;
		rectTransform->height = 50;

		BuildingSelectButton02->AddComponent<Button>()->AddEvent(
			[](void*) {
				BuildManager::buildManager->SelectModel(ASSET MESH("Cube"), ASSET MATERIAL("bricksMat"), 5);
			});
		{
			auto textobject = BuildingSelectButton02->AddChildUI();
			auto rectTransform = textobject->GetComponent<RectTransform>();
			rectTransform->anchorMin = { 0, 0 };
			rectTransform->anchorMax = { 1, 1 };

			Text* text = textobject->AddComponent<Text>();
			text->text = L"주거건물";
			text->fontSize = 10;
			text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
			text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			textObjects.push_back(textobject);
		}
	}

	auto BuildingSelectButton03 = CreateImage();
	{
		auto rectTransform = BuildingSelectButton03->GetComponent<RectTransform>();
		rectTransform->anchorMin = { 0.5, 0 };
		rectTransform->anchorMax = { 0.5, 0 };
		rectTransform->pivot = { 0.5, 0 };
		rectTransform->posX = -70;
		rectTransform->posY = 10;
		rectTransform->width = 50;
		rectTransform->height = 50;

		BuildingSelectButton03->AddComponent<Button>()->AddEvent(
			[](void*) {
				BuildManager::buildManager->SelectModel(ASSET MESH("Sphere"), ASSET MATERIAL("bricksMat"), 1);
			});
		{
			auto textobject = BuildingSelectButton03->AddChildUI();
			auto rectTransform = textobject->GetComponent<RectTransform>();
			rectTransform->anchorMin = { 0, 0 };
			rectTransform->anchorMax = { 1, 1 };

			Text* text = textobject->AddComponent<Text>();
			text->text = L"테마건물";
			text->fontSize = 10;
			text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
			text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			textObjects.push_back(textobject);
		}
	}

	auto BuildingSelectButton04 = CreateImage();
	{
		auto rectTransform = BuildingSelectButton04->GetComponent<RectTransform>();
		rectTransform->anchorMin = { 0.5, 0 };
		rectTransform->anchorMax = { 0.5, 0 };
		rectTransform->pivot = { 0.5, 0 };
		rectTransform->posX = -10;
		rectTransform->posY = 10;
		rectTransform->width = 50;
		rectTransform->height = 50;

		BuildingSelectButton04->AddComponent<Button>()->AddEvent(
			[](void*) {
				BuildManager::buildManager->SelectModel(ASSET MESH("Cube"), ASSET MATERIAL("bricksMat"), 5);
			});
		{
			auto textobject = BuildingSelectButton04->AddChildUI();
			auto rectTransform = textobject->GetComponent<RectTransform>();
			rectTransform->anchorMin = { 0, 0 };
			rectTransform->anchorMax = { 1, 1 };

			Text* text = textobject->AddComponent<Text>();
			text->text = L"조경";
			text->fontSize = 10;
			text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
			text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			textObjects.push_back(textobject);
		}
	}
}