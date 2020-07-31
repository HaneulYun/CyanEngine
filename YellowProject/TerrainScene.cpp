#include "pch.h"
#include "TerrainScene.h"

#define TAG_DAFULT 0
#define TAG_BUILDING 1

BuildingBuilder* BuildingBuilder::buildingBuilder{ nullptr };

void TerrainScene::BuildObjects()
{
	spatialPartitioningManager.tagData.AddTag("building");
	spatialPartitioningManager.tagData.SetTagCollision(TAG_BUILDING, TAG_BUILDING, true);


	///*** Asset ***///
	//LoadTextureAsset();
	//LoadMaterialAsset();
	//LoadMeshAsset();
	//
	//LoadPrefab();
	//*** Texture ***//
	{
		ASSET AddTexture("none", L"Textures\\none.dds");
		ASSET AddTexture("polyArtTex", L"Textures\\PolyArtTex.dds");
		ASSET AddTexture("bricksTex", L"Textures\\bricks2.dds");
		ASSET AddTexture("stoneTex", L"Textures\\stone.dds");
		ASSET AddTexture("tileTex", L"Textures\\tile.dds");
		ASSET AddTexture("tree", L"Textures\\tree01S.dds");
		ASSET AddTexture("grass", L"Textures\\grass01.dds");
		ASSET AddTexture("ground", L"Textures\\ground.dds");

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
		ASSET AddMaterial("grass",		ASSET TEXTURE("grass"), 0, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.01f, 0.01f, 0.01f }, 0.9f);
		ASSET AddMaterial("ground", ASSET TEXTURE("ground"), nullptr, { 0.48f, 0.64f, 0.2f, 1.0f }, { 0.01f, 0.01f, 0.01f }, 0.9f, Matrix4x4::MatrixScaling(200, 200, 200));

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
		ASSET AddMesh("Plane", Mesh::CreatePlane());
		ASSET AddFbxForAnimation("ApprenticeSK", "Models\\modelTest.fbx");

		//ASSET AddFbxForMesh("SM_Well", "Assets\\AdvancedVillagePack\\Meshes\\SM_Well.FBX");
		//ASSET AddFbxForMesh("SM_Well_Extra02", "Assets\\AdvancedVillagePack\\Meshes\\SM_Well_Extra02.FBX");
		//ASSET AddFbxForMesh("SM_Well_Extra03", "Assets\\AdvancedVillagePack\\Meshes\\SM_Well_Extra03.FBX");
		ASSET AddFbxForMesh("SM_House_Var01", "Assets\\AdvancedVillagePack\\Meshes\\SM_House_Var01.FBX");
		//ASSET AddFbxForMesh("SM_House_Var01", "Assets\\AdvancedVillagePack\\Meshes\\Pillar.FBX");
		//ASSET AddFbxForMesh("SM_House_Var02", "Assets\\AdvancedVillagePack\\Meshes\\SM_House_Var02.FBX");
		//ASSET AddFbxForMesh("SM_House_Var02_Extra", "Assets\\AdvancedVillagePack\\Meshes\\SM_House_Var02_Extra.FBX");
		ASSET AddFbxForMesh("SM_Mushroom_Var01", "Assets\\AdvancedVillagePack\\Meshes\\SM_Mushroom_Var01.FBX");
		ASSET AddFbxForMesh("SM_Mushroom_Var02", "Assets\\AdvancedVillagePack\\Meshes\\SM_Mushroom_Var02.FBX");
		ASSET AddFbxForMesh("SM_Mushroom_Var03", "Assets\\AdvancedVillagePack\\Meshes\\SM_Mushroom_Var03.FBX");
		ASSET AddFbxForMesh("SM_Mushroom_Var04", "Assets\\AdvancedVillagePack\\Meshes\\SM_Mushroom_Var04.FBX");
		ASSET AddFbxForMesh("SM_Mushroom_Var05", "Assets\\AdvancedVillagePack\\Meshes\\SM_Mushroom_Var05.FBX");
		ASSET AddFbxForMesh("SM_Mushroom_Var06", "Assets\\AdvancedVillagePack\\Meshes\\SM_Mushroom_Var06.FBX");
		//ASSET AddFbxForMesh("SM_Cart_Var01", "Assets\\AdvancedVillagePack\\Meshes\\SM_Cart_Var01.FBX");
		//ASSET AddFbxForMesh("SM_Cart_Var02", "Assets\\AdvancedVillagePack\\Meshes\\SM_Cart_Var02.FBX");
		//ASSET AddFbxForMesh("SM_Barrel", "Assets\\AdvancedVillagePack\\Meshes\\SM_Barrel.FBX");
		//ASSET AddFbxForMesh("SM_Bucket", "Assets\\AdvancedVillagePack\\Meshes\\SM_Bucket.FBX");
		//ASSET AddFbxForMesh("SM_Fence_Var01", "Assets\\AdvancedVillagePack\\Meshes\\SM_Fence_Var01.FBX");
		//ASSET AddFbxForMesh("SM_Fence_Var02", "Assets\\AdvancedVillagePack\\Meshes\\SM_Fence_Var02.FBX");
		//ASSET AddFbxForMesh("SM_Fence_Var03", "Assets\\AdvancedVillagePack\\Meshes\\SM_Fence_Var03.FBX");
		//ASSET AddFbxForMesh("SM_Fence_Var04", "Assets\\AdvancedVillagePack\\Meshes\\SM_Fence_Var04.FBX");
		//ASSET AddFbxForMesh("SM_Cauldron", "Assets\\AdvancedVillagePack\\Meshes\\SM_Cauldron.FBX");
		//ASSET AddFbxForMesh("SM_Spike", "Assets\\AdvancedVillagePack\\Meshes\\SM_Spike.FBX");
		//ASSET AddFbxForMesh("SM_Fish", "Assets\\AdvancedVillagePack\\Meshes\\SM_Fish.FBX");
		//ASSET AddFbxForMesh("SM_Pot_Var01", "Assets\\AdvancedVillagePack\\Meshes\\SM_Pot_Var01.FBX");
		//ASSET AddFbxForMesh("SM_Pot_Var02", "Assets\\AdvancedVillagePack\\Meshes\\SM_Pot_Var02.FBX");
		//ASSET AddFbxForMesh("SM_Pot_Var03", "Assets\\AdvancedVillagePack\\Meshes\\SM_Pot_Var03.FBX");
		//ASSET AddFbxForMesh("SM_Pot_Var04", "Assets\\AdvancedVillagePack\\Meshes\\SM_Pot_Var04.FBX");
		//ASSET AddFbxForMesh("SM_Pot_Var05", "Assets\\AdvancedVillagePack\\Meshes\\SM_Pot_Var05.FBX");
		//ASSET AddFbxForMesh("SM_Pot_Var06", "Assets\\AdvancedVillagePack\\Meshes\\SM_Pot_Var06.FBX");
		//ASSET AddFbxForMesh("SM_Apple", "Assets\\AdvancedVillagePack\\Meshes\\SM_Apple.FBX");
		//ASSET AddFbxForMesh("SM_Potato", "Assets\\AdvancedVillagePack\\Meshes\\SM_Potato.FBX");
		//ASSET AddFbxForMesh("SM_Tomato", "Assets\\AdvancedVillagePack\\Meshes\\SM_Tomato.FBX");
		//ASSET AddFbxForMesh("SM_Watermellon", "Assets\\AdvancedVillagePack\\Meshes\\SM_Watermellon.FBX");
		//ASSET AddFbxForMesh("SM_Sack_Apple", "Assets\\AdvancedVillagePack\\Meshes\\SM_Sack_Apple.FBX");
		//ASSET AddFbxForMesh("SM_Sack_Flour", "Assets\\AdvancedVillagePack\\Meshes\\SM_Sack_Flour.FBX");
		//ASSET AddFbxForMesh("SM_Sack_Potato", "Assets\\AdvancedVillagePack\\Meshes\\SM_Sack_Potato.FBX");
		//ASSET AddFbxForMesh("SM_Sack_Tomato", "Assets\\AdvancedVillagePack\\Meshes\\SM_Sack_Tomato.FBX");
		//ASSET AddFbxForMesh("SM_Sack_Var01", "Assets\\AdvancedVillagePack\\Meshes\\SM_Sack_Var01.FBX");
		//ASSET AddFbxForMesh("SM_Sack_Var02", "Assets\\AdvancedVillagePack\\Meshes\\SM_Sack_Var02.FBX");
		ASSET AddFbxForMesh("SM_StreetLight_LightsOn", "Assets\\AdvancedVillagePack\\Meshes\\SM_StreetLight_LightsOn.FBX");
	}

	{
		auto prefab = CreateEmptyPrefab();
		prefab->AddComponent<MeshFilter>();
		prefab->AddComponent<Renderer>();
		prefab->AddComponent<BoxCollider>();

		ASSET AddPrefab("MRC", std::make_unique<GameObject>(prefab));
	}

	{
		auto prefab = CreateEmptyPrefab();

		{
			auto child = prefab->AddChild();
			child->transform->Rotate({ 1.0,0.0,0.0 }, -90.0f);
			child->AddComponent<MeshFilter>()->mesh = ASSET MESH("SM_StreetLight_LightsOn");
			child->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("material_03"));
			child->AddComponent<BoxCollider>();
		}
		
		{
			auto spotLight = prefab->AddChild();
			spotLight->transform->position = { 0.0f, 2.5f, 1.25f };
			//spotLight->GetComponent<Transform>()->Rotate({ 1, 0, 0 }, 90);
			spotLight->AddComponent<Light>()->Strength = { 1, 1, 1 };
			spotLight->GetComponent<Light>()->type = Light::Type::Point;
			spotLight->GetComponent<Light>()->FalloffEnd = 5;
		}

		ASSET AddPrefab("StreetLight", std::make_unique<GameObject>(prefab));
	}

	///*** Game Object ***///

	ASSET AddAudioClip("testSound", "Assets\\FootstepSound\\Grass\\test.mp3");

	auto soundBox = CreateEmpty();
	{
		soundBox->transform->position = { 512, 35, 1024 * 0.45 };
		soundBox->transform->Scale({10, 10, 10});
		soundBox->AddComponent<MeshFilter>()->mesh = ASSET MESH("Cube");
		soundBox->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("none"));
		soundBox->AddComponent<AudioSource>()->clip = ASSET AUDIO_CLIP("testSound");
	}

	auto mainCamera = CreateEmpty();
	{
		mainCamera->transform->position = { 1024 * 0.5, 50, 1024 * 0.4 };
		camera = camera->main = mainCamera->AddComponent<Camera>();
		mainCamera->AddComponent<CameraController>();
		mainCamera->AddComponent<AudioListener>();
	}

	auto directionalLight = CreateEmpty();
	{
		directionalLight->transform->Rotate({ 1, 0, 0 }, 90);
		auto light = directionalLight->AddComponent<Light>();
		light->Strength = { 0.9f, 0.8f, 0.7f };
		light->shadowType = Light::Shadows;

		directionalLight->AddComponent<RotatingBehavior>()->setAxisAndSpeed({ 0, 1, 0 }, 360 * 0.05);
	}

	auto cubeObject = CreateEmpty();
	{
		cubeObject->transform->position = { 512, 50, 512 };
		cubeObject->transform->Scale({200, 1, 200});
		cubeObject->AddComponent<MeshFilter>()->mesh = ASSET MESH("Cube");
		cubeObject->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("none"));
	}

	//GameObject* pointLight0 = CreateEmpty();
	//{
	//	pointLight0->GetComponent<Transform>()->position = { 540, 27, 540 };
	//	pointLight0->AddComponent<Light>()->Strength = { 1, 1, 1 };
	//	pointLight0->GetComponent<Light>()->type = Light::Point;
	//	pointLight0->GetComponent<Light>()->FalloffEnd = 10;
	//}
	//GameObject* spotLight = CreateEmpty();
	//{
	//	spotLight->GetComponent<Transform>()->position = { 570, 25, 540 };
	//	spotLight->GetComponent<Transform>()->Rotate({ 1, 0, 0 }, 90);
	//	spotLight->AddComponent<Light>()->Strength = { 1, 1, 1 };
	//	spotLight->GetComponent<Light>()->type = Light::Spot;
	//	spotLight->GetComponent<Light>()->FalloffEnd = 30;
	//}
	//GameObject* spotLight1 = CreateEmpty();
	//{
	//	spotLight1->GetComponent<Transform>()->position = { 540, 23, 510 };
	//	spotLight1->GetComponent<Transform>()->Rotate({ 1, 0, 0 }, 90);
	//	spotLight1->AddComponent<Light>()->Strength = { 1, 1, 1 };
	//	spotLight1->GetComponent<Light>()->type = Light::Spot;
	//	spotLight1->GetComponent<Light>()->FalloffEnd = 50;
	//}

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

	auto terrainBottom = CreateEmpty();
	{
		terrainBottom->transform->Scale({ 1024, 1, 1024 });
		terrainBottom->transform->Rotate({ 0, 0, 1 }, 180);
		terrainBottom->transform->position = { 512, 0, 512 };
		terrainBottom->AddComponent<MeshFilter>()->mesh = ASSET MESH("Plane");
		terrainBottom->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("none"));
	}

	auto object = CreateUI();
	{
		auto rt = object->GetComponent<RectTransform>();
		rt->width = CyanFW::Instance()->GetWidth();
		rt->height = CyanFW::Instance()->GetHeight();

		auto buildingBuilder = object->AddComponent<BuildingBuilder>();
		buildingBuilder->serializeBuildings();
		buildingBuilder->terrain = terrainData;

		auto buildingTypeSelector = object->AddComponent<BuildingTypeSelector>();
		buildingTypeSelector->builder = buildingBuilder;

		buildingTypeSelector->addBuildingType(BuildingBuilder::Landmark, L"랜드\n마크", -80, 0);
		buildingTypeSelector->addBuildingType(BuildingBuilder::House, L"주거\n건물", -40, 0);
		buildingTypeSelector->addBuildingType(BuildingBuilder::Theme, L"테마\n건물", 0, 0);
		buildingTypeSelector->addBuildingType(BuildingBuilder::Landscape, L"조경", 40, 0);
		buildingTypeSelector->addBuildingType(BuildingBuilder::Prop, L"소품", 80, 0);
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