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

		ASSET AddTexture("material_01", L"Assets\\AdvancedVillagePack\\Textures\\T_Pack_01_D.dds");
		ASSET AddTexture("material_02", L"Assets\\AdvancedVillagePack\\Textures\\T_Pack_02_D.dds");
		ASSET AddTexture("material_03", L"Assets\\AdvancedVillagePack\\Textures\\T_Pack_03_D.dds");
		ASSET AddTexture("house01_D", L"Assets\\AdvancedVillagePack\\Textures\\T_Pack_04_D.dds");
		ASSET AddTexture("house01_N", L"Assets\\AdvancedVillagePack\\Textures\\T_Pack_04_N.dds");
		ASSET AddTexture("house02", L"Assets\\AdvancedVillagePack\\Textures\\T_Pack_09_D.dds");
	}

	//*** Material ***//
	{
		ASSET AddMaterial("none",		ASSET TEXTURE("none"));
		ASSET AddMaterial("PolyArt",	ASSET TEXTURE("polyArtTex"), 0, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.01f, 0.01f, 0.01f }, 0.9f);
		ASSET AddMaterial("bricksMat",	ASSET TEXTURE("bricksTex"), 0, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.02f, 0.02f, 0.02f }, 0.1f);
		ASSET AddMaterial("stoneMat",	ASSET TEXTURE("none"), 0, { 0.0f, 0.0f, 0.1f, 1.0f }, { 0.98f, 0.97f, 0.95f }, 0.1f);
		ASSET AddMaterial("tile0",		ASSET TEXTURE("tileTex"), 0, { 0.9f, 0.9f, 0.9f, 1.0f }, { 0.02f, 0.02f, 0.02f }, 0.1f, Matrix4x4::MatrixScaling(8, 8, 1));
		ASSET AddMaterial("tree0",		ASSET TEXTURE("tree"), 0, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.01f, 0.01f, 0.01f }, 0.1f);
		ASSET AddMaterial("grass",		ASSET TEXTURE("grass"), 0, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.01f, 0.01f, 0.01f }, 0.1f);

		ASSET AddMaterial("material_01", ASSET TEXTURE("material_01"), 0, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.01f, 0.01f, 0.01f }, 0.9f);
		ASSET AddMaterial("material_02", ASSET TEXTURE("material_02"), 0, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.01f, 0.01f, 0.01f }, 0.9f);
		ASSET AddMaterial("material_03", ASSET TEXTURE("material_03"), 0, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.01f, 0.01f, 0.01f }, 0.9f);
		ASSET AddMaterial("house01", ASSET TEXTURE("house01_D"), ASSET TEXTURE("house01_N"), { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.01f, 0.01f, 0.01f }, 0.9f);
		ASSET AddMaterial("house02", ASSET TEXTURE("house02"), 0, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.01f, 0.01f, 0.01f }, 0.9f);
	}

	//*** Mesh ***//
	{
		ASSET AddMesh("Cube", Mesh::CreateCube());
		ASSET AddMesh("Quad", Mesh::CreateQuad());
		ASSET AddMesh("Cylinder", Mesh::CreateCylinder());
		ASSET AddFbxForAnimation("ApprenticeSK", "Models\\modelTest.fbx");

		ASSET AddFbxForMesh("SM_Well", "Assets\\AdvancedVillagePack\\Meshes\\SM_Well.FBX");
		ASSET AddFbxForMesh("SM_Well_Extra02", "Assets\\AdvancedVillagePack\\Meshes\\SM_Well_Extra02.FBX");
		ASSET AddFbxForMesh("SM_Well_Extra03", "Assets\\AdvancedVillagePack\\Meshes\\SM_Well_Extra03.FBX");
		ASSET AddFbxForMesh("SM_House_Var01", "Assets\\AdvancedVillagePack\\Meshes\\SM_House_Var01.FBX");
		ASSET AddFbxForMesh("SM_House_Var02", "Assets\\AdvancedVillagePack\\Meshes\\SM_House_Var02.FBX");
		ASSET AddFbxForMesh("SM_House_Var02_Extra", "Assets\\AdvancedVillagePack\\Meshes\\SM_House_Var02_Extra.FBX");
		ASSET AddFbxForMesh("SM_Mushroom_Var01", "Assets\\AdvancedVillagePack\\Meshes\\SM_Mushroom_Var01.FBX");
		ASSET AddFbxForMesh("SM_Mushroom_Var02", "Assets\\AdvancedVillagePack\\Meshes\\SM_Mushroom_Var02.FBX");
		ASSET AddFbxForMesh("SM_Mushroom_Var03", "Assets\\AdvancedVillagePack\\Meshes\\SM_Mushroom_Var03.FBX");
		ASSET AddFbxForMesh("SM_Mushroom_Var04", "Assets\\AdvancedVillagePack\\Meshes\\SM_Mushroom_Var04.FBX");
		ASSET AddFbxForMesh("SM_Mushroom_Var05", "Assets\\AdvancedVillagePack\\Meshes\\SM_Mushroom_Var05.FBX");
		ASSET AddFbxForMesh("SM_Mushroom_Var06", "Assets\\AdvancedVillagePack\\Meshes\\SM_Mushroom_Var06.FBX");
		ASSET AddFbxForMesh("SM_Cart_Var01", "Assets\\AdvancedVillagePack\\Meshes\\SM_Cart_Var01.FBX");
		ASSET AddFbxForMesh("SM_Cart_Var02", "Assets\\AdvancedVillagePack\\Meshes\\SM_Cart_Var02.FBX");
		ASSET AddFbxForMesh("SM_Barrel", "Assets\\AdvancedVillagePack\\Meshes\\SM_Barrel.FBX");
		ASSET AddFbxForMesh("SM_Bucket", "Assets\\AdvancedVillagePack\\Meshes\\SM_Bucket.FBX");
		ASSET AddFbxForMesh("SM_Fence_Var01", "Assets\\AdvancedVillagePack\\Meshes\\SM_Fence_Var01.FBX");
		ASSET AddFbxForMesh("SM_Fence_Var02", "Assets\\AdvancedVillagePack\\Meshes\\SM_Fence_Var02.FBX");
		ASSET AddFbxForMesh("SM_Fence_Var03", "Assets\\AdvancedVillagePack\\Meshes\\SM_Fence_Var03.FBX");
		ASSET AddFbxForMesh("SM_Fence_Var04", "Assets\\AdvancedVillagePack\\Meshes\\SM_Fence_Var04.FBX");
		ASSET AddFbxForMesh("SM_Cauldron", "Assets\\AdvancedVillagePack\\Meshes\\SM_Cauldron.FBX");
		ASSET AddFbxForMesh("SM_Spike", "Assets\\AdvancedVillagePack\\Meshes\\SM_Spike.FBX");
		ASSET AddFbxForMesh("SM_Fish", "Assets\\AdvancedVillagePack\\Meshes\\SM_Fish.FBX");
		ASSET AddFbxForMesh("SM_Pot_Var01", "Assets\\AdvancedVillagePack\\Meshes\\SM_Pot_Var01.FBX");
		ASSET AddFbxForMesh("SM_Pot_Var02", "Assets\\AdvancedVillagePack\\Meshes\\SM_Pot_Var02.FBX");
		ASSET AddFbxForMesh("SM_Pot_Var03", "Assets\\AdvancedVillagePack\\Meshes\\SM_Pot_Var03.FBX");
		ASSET AddFbxForMesh("SM_Pot_Var04", "Assets\\AdvancedVillagePack\\Meshes\\SM_Pot_Var04.FBX");
		ASSET AddFbxForMesh("SM_Pot_Var05", "Assets\\AdvancedVillagePack\\Meshes\\SM_Pot_Var05.FBX");
		ASSET AddFbxForMesh("SM_Pot_Var06", "Assets\\AdvancedVillagePack\\Meshes\\SM_Pot_Var06.FBX");
		ASSET AddFbxForMesh("SM_Apple", "Assets\\AdvancedVillagePack\\Meshes\\SM_Apple.FBX");
		ASSET AddFbxForMesh("SM_Potato", "Assets\\AdvancedVillagePack\\Meshes\\SM_Potato.FBX");
		ASSET AddFbxForMesh("SM_Tomato", "Assets\\AdvancedVillagePack\\Meshes\\SM_Tomato.FBX");
		ASSET AddFbxForMesh("SM_Watermellon", "Assets\\AdvancedVillagePack\\Meshes\\SM_Watermellon.FBX");
		ASSET AddFbxForMesh("SM_Sack_Apple", "Assets\\AdvancedVillagePack\\Meshes\\SM_Sack_Apple.FBX");
		ASSET AddFbxForMesh("SM_Sack_Flour", "Assets\\AdvancedVillagePack\\Meshes\\SM_Sack_Flour.FBX");
		ASSET AddFbxForMesh("SM_Sack_Potato", "Assets\\AdvancedVillagePack\\Meshes\\SM_Sack_Potato.FBX");
		ASSET AddFbxForMesh("SM_Sack_Tomato", "Assets\\AdvancedVillagePack\\Meshes\\SM_Sack_Tomato.FBX");
		ASSET AddFbxForMesh("SM_Sack_Var01", "Assets\\AdvancedVillagePack\\Meshes\\SM_Sack_Var01.FBX");
		ASSET AddFbxForMesh("SM_Sack_Var02", "Assets\\AdvancedVillagePack\\Meshes\\SM_Sack_Var02.FBX");
	}

	///*** Game Object ***///

	GameObject* mainCamera = CreateEmpty();
	{
		mainCamera->transform->position = { 1080 * 0.5, 256 * 0.2, 1080 * 0.4 };
		camera = camera->main = mainCamera->AddComponent<Camera>();
		mainCamera->AddComponent<CameraController>();
	}

	//GameObject* directionalLight = CreateEmpty();
	//{
	//	directionalLight->AddComponent<Light>()->Strength = { 0.9f, 0.8f, 0.7f };
	//	directionalLight->AddComponent<Updater>()->Set(
	//		&directionalLight->GetComponent<Transform>()->forward,
	//		&Graphics::Instance()->rotatedLightDirections[0], sizeof(Vector3));
	//}
	//GameObject* directionalLight2 = CreateEmpty();
	//{
	//	directionalLight2->GetComponent<Transform>()->forward = { -0.57735f, -0.57735f, 0.57735f };
	//	directionalLight2->AddComponent<Light>()->Strength = { 0.4f, 0.4f, 0.4f };
	//}
	//GameObject* directionalLight3 = CreateEmpty();
	//{
	//	directionalLight3->GetComponent<Transform>()->forward = { 0.0f, -0.707f, -0.707f };
	//	directionalLight3->AddComponent<Light>()->Strength = { 0.2f, 0.2f, 0.2f };
	//}

	GameObject* pointLight0 = CreateEmpty();
	{
		pointLight0->GetComponent<Transform>()->position = { 540, 27, 540 };
		pointLight0->AddComponent<Light>()->Strength = { 1, 1, 1 };
		pointLight0->GetComponent<Light>()->type = Light::Type::Point;
		pointLight0->GetComponent<Light>()->FalloffEnd = 10;
	}
	GameObject* spotLight = CreateEmpty();
	{
		spotLight->GetComponent<Transform>()->position = { 570, 25, 540 };
		spotLight->GetComponent<Transform>()->Rotate({ 1, 0, 0 }, 90);
		spotLight->AddComponent<Light>()->Strength = { 1, 1, 1 };
		spotLight->GetComponent<Light>()->type = Light::Type::Spot;
		spotLight->GetComponent<Light>()->FalloffEnd = 30;
	}
	GameObject* spotLight1 = CreateEmpty();
	{
		spotLight1->GetComponent<Transform>()->position = { 540, 23, 510 };
		spotLight1->GetComponent<Transform>()->Rotate({ 1, 0, 0 }, 90);
		spotLight1->AddComponent<Light>()->Strength = { 1, 1, 1 };
		spotLight1->GetComponent<Light>()->type = Light::Type::Spot;
		spotLight1->GetComponent<Light>()->FalloffEnd = 50;
	}

	//GameObject* debug = CreateEmpty();
	//{
	//	debug->AddComponent<MeshFilter>()->mesh = ASSET MESH("Quad");
	//	debug->layer = (int)RenderLayer::Debug;
	//}

	{
		auto ritem = CreateEmpty();
		ritem->GetComponent<Transform>()->Scale({ 5000.0f, 5000.0f, 5000.0f });
		auto mesh = ritem->AddComponent<MeshFilter>()->mesh = ASSET MESH("Sphere");
		auto renderer = ritem->AddComponent<Renderer>();
		for (auto& sm : mesh->DrawArgs)
			renderer->materials.push_back(ASSET MATERIAL("none"));
		ritem->layer = (int)RenderLayer::Sky;
	}

	float TerrainSize = 1081;

	GameObject* terrain = CreateEmpty();
	auto terrainData = terrain->AddComponent<Terrain>();
	{
		{
			terrainData->terrainData.AlphamapTextureName = L"Texture\\heightMap_HN.raw";
			terrainData->terrainData.heightmapHeight = TerrainSize;
			terrainData->terrainData.heightmapWidth = TerrainSize;

			terrainData->terrainData.size = { TerrainSize, 255, TerrainSize };

			terrainData->Set();
		}
		terrain->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("PolyArt"));
	}

	{
		GameObject* manager = CreateEmpty();
		BuildManager* buildManager = manager->AddComponent<BuildManager>();
		buildManager->terrain = terrain;
		buildManager->heightMap = &terrainData->terrainData;
		buildManager->terrainMesh = terrainData->terrainData.heightmapTexture;

		buildManager->Load();

		//bm->SelectModel(geometries["Cube"].get(), 8, 5);
		BuildManager::buildManager = buildManager;
		ButtonManager* buttonManager = manager->AddComponent<ButtonManager>();
		ButtonManager::buttonManager = buttonManager;
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
		rectTransform->setAnchorAndPivot(0.5, 0);
		rectTransform->setPosAndSize(-70, 70, 50, 50);

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
		rectTransform->setAnchorAndPivot(0.5, 0);
		rectTransform->setPosAndSize(-70, 10, 50, 50);

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
		rectTransform->setAnchorAndPivot(0.5, 0);
		rectTransform->setPosAndSize(-10, 10, 50, 50);

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
		rectTransform->setAnchorAndPivot(0.5, 0);
		rectTransform->setPosAndSize(-70, 10, 50, 50);

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
		rectTransform->setAnchorAndPivot(0.5, 0);
		rectTransform->setPosAndSize(-10, 10, 50, 50);

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