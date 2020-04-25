#include "pch.h"
#include "TerrainScene.h"

void TerrainScene::BuildObjects()
{
	///*** Asset ***///
	//*** Texture ***//
	{
		AddTexture(0, "none", L"Textures\\none.dds");
		AddTexture(1, "polyArtTex", L"Textures\\PolyArtTex.dds");
		AddTexture(2, "bricksTex", L"Textures\\bricks2.dds");
		AddTexture(3, "stoneTex", L"Textures\\stone.dds");
		AddTexture(4, "tileTex", L"Textures\\tile.dds");
	}

	//*** Material ***//
	{
		for (int i = 5; i < 10; ++i)
		{
			auto material = std::make_unique<Material>();
			material->Name = "material_" + std::to_string(i);
			material->MatCBIndex = i;
			material->DiffuseSrvHeapIndex = 0;
			material->NormalSrvHeapIndex = 0;
			material->DiffuseAlbedo = RANDOM_COLOR;
			material->FresnelR0 = Vector3(0.98f, 0.97f, 0.95f);
			material->Roughness = 0.0f;
			materials[material->Name] = std::move(material);
		}

		{
			auto material = std::make_unique<Material>();
			material->Name = "PolyArt";
			material->MatCBIndex = 0;
			material->DiffuseSrvHeapIndex = 1;
			material->FresnelR0 = { 0.01f, 0.01f, 0.01f };
			material->Roughness = 0.9f;
			materials[material->Name] = std::move(material);

			auto material_bricks0 = std::make_unique<Material>();
			material_bricks0->Name = "bricksMat";
			material_bricks0->MatCBIndex = 1;
			material_bricks0->DiffuseSrvHeapIndex = 2;
			material_bricks0->FresnelR0 = Vector3(0.02f, 0.02f, 0.02f);
			material_bricks0->Roughness = 0.1f;
			materials[material_bricks0->Name] = std::move(material_bricks0);

			auto material_stone0 = std::make_unique<Material>();
			material_stone0->Name = "stoneMat";
			material_stone0->MatCBIndex = 2;
			material_stone0->DiffuseSrvHeapIndex = 0;
			material_stone0->DiffuseAlbedo = Vector4(0.0f, 0.0f, 0.1f, 1.0f);
			material_stone0->FresnelR0 = Vector3(0.98f, 0.97f, 0.95f);
			material_stone0->Roughness = 0.1f;
			materials[material_stone0->Name] = std::move(material_stone0);

			auto material_tile0 = std::make_unique<Material>();
			material_tile0->Name = "tile0";
			material_tile0->MatCBIndex = 3;
			material_tile0->DiffuseSrvHeapIndex = 4;
			material_tile0->DiffuseAlbedo = Vector4(0.9f, 0.9f, 0.9f, 1.0f);
			material_tile0->FresnelR0 = Vector3(0.2f, 0.2f, 0.2f);
			material_tile0->Roughness = 0.1f;
			material_tile0->MatTransform = Matrix4x4::MatrixScaling(8, 8, 1);
			materials[material_tile0->Name] = std::move(material_tile0);
		}

		{
			auto material = std::make_unique<Material>();
			material->Name = "sky";
			material->MatCBIndex = 4;
			material->DiffuseSrvHeapIndex = 5;
			material->DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
			material->FresnelR0 = Vector3(0.1f, 0.1f, 0.1f);
			material->Roughness = 1.0f;
			materials[material->Name] = std::move(material);
		}
	}
	
	//*** Mesh ***//
	{
		geometries["Image"] = Mesh::CreateQuad();
		geometries["Cube"] = Mesh::CreateCube();
		geometries["Plane"] = Mesh::CreatePlane();
		geometries["Sphere"] = Mesh::CreateSphere();
		geometries["Cylinder"] = Mesh::CreateCylinder();
		AddFbxForAnimation("ApprenticeSK", "Models\\modelTest.fbx");
	}

	CHeightMapImage* m_pHeightMapImage = new CHeightMapImage(L"Texture\\heightMap.raw", 257, 257, { 1.0f, 0.1f, 1.0f });
	CHeightMapGridMesh* gridMesh = new CHeightMapGridMesh(0, 0, 257, 257, { 1, 1, 1 }, { 1, 1, 0, 1 }, m_pHeightMapImage);

	//*** Animation ***//
	{
		AddFbxForAnimation("Attack01_BowAnim", "Models\\BowStance\\Attack01_BowAnim.fbx");
		AddFbxForAnimation("Attack01Maintain_BowAnim", "Models\\BowStance\\Attack01Maintain_BowAnim.fbx");
		AddFbxForAnimation("Attack01RepeatFire_BowAnim", "Models\\BowStance\\Attack01RepeatFire_BowAnim.fbx");
		AddFbxForAnimation("Attack01Start_BowAnim", "Models\\BowStance\\Attack01Start_BowAnim.fbx");
		AddFbxForAnimation("Attack02Maintain_BowAnim", "Models\\BowStance\\Attack02Maintain_BowAnim.fbx");
		AddFbxForAnimation("Attack02RepeatFire_BowAnim", "Models\\BowStance\\Attack02RepeatFire_BowAnim.fbx");
		AddFbxForAnimation("Attack02Start_BowAnim", "Models\\BowStance\\Attack02Start_BowAnim.fbx");
		AddFbxForAnimation("DashBackward_BowAnim", "Models\\BowStance\\DashBackward_BowAnim.fbx");
		AddFbxForAnimation("DashForward_BowAnim", "Models\\BowStance\\DashForward_BowAnim.fbx");

		AddFbxForAnimation("Walk_BowAnim", "Models\\BowStance\\Walk_BowAnim.fbx");
		AddFbxForAnimation("WalkBack_BowAnim", "Models\\BowStance\\WalkBack_BowAnim.fbx");
		AddFbxForAnimation("WalkRight_BowAnim", "Models\\BowStance\\WalkRight_BowAnim.fbx");
		AddFbxForAnimation("WalkLeft_BowAnim", "Models\\BowStance\\WalkLeft_BowAnim.fbx");
		AddFbxForAnimation("Idle_BowAnim", "Models\\BowStance\\Idle_BowAnim.fbx");
	}

	AnimatorController* controller = new AnimatorController();
	//*** AnimatorController ***//
	{
		controller->AddState("Attack01_BowAnim", animationClips["Attack01_BowAnim"].get());
		controller->AddState("Attack01Maintain_BowAnim", animationClips["Attack01Maintain_BowAnim"].get());
		controller->AddState("Attack01RepeatFire_BowAnim", animationClips["Attack01RepeatFire_BowAnim"].get());
		controller->AddState("Attack01Start_BowAnim", animationClips["Attack01Start_BowAnim"].get());
		controller->AddState("Attack02Maintain_BowAnim", animationClips["Attack02Maintain_BowAnim"].get());
		controller->AddState("Attack02RepeatFire_BowAnim", animationClips["Attack02RepeatFire_BowAnim"].get());
		controller->AddState("Attack02Start_BowAnim", animationClips["Attack02Start_BowAnim"].get());
		controller->AddState("DashBackward_BowAnim", animationClips["DashBackward_BowAnim"].get());
		controller->AddState("DashForward_BowAnim", animationClips["DashForward_BowAnim"].get());

		controller->AddParameterFloat("Speed");
		controller->AddParameterFloat("HoriSpeed");

		controller->AddState("Idle",		animationClips["Idle_BowAnim"].get());
		controller->AddState("Walk",		animationClips["Walk_BowAnim"].get());
		controller->AddState("WalkBack",	animationClips["WalkBack_BowAnim"].get());
		controller->AddState("WalkRight",	animationClips["WalkRight_BowAnim"].get());
		controller->AddState("WalkLeft",	animationClips["WalkLeft_BowAnim"].get());

		controller->AddTransition("Idle", "Walk",		TransitionCondition::CreateFloat("Speed", Greater, 0.1));
		controller->AddTransition("Idle", "WalkBack",	TransitionCondition::CreateFloat("Speed", Less, -0.1));
		controller->AddTransition("Walk", "Idle",		TransitionCondition::CreateFloat("Speed", Less, 0.1));
		controller->AddTransition("WalkBack", "Idle",	TransitionCondition::CreateFloat("Speed", Greater, -0.1));

		controller->AddTransition("Idle", "WalkLeft",	TransitionCondition::CreateFloat("HoriSpeed", Greater, 0.1));
		controller->AddTransition("Idle", "WalkRight",	TransitionCondition::CreateFloat("HoriSpeed", Less, -0.1));
		controller->AddTransition("WalkLeft", "Idle",	TransitionCondition::CreateFloat("HoriSpeed", Less, 0.1));
		controller->AddTransition("WalkRight", "Idle",	TransitionCondition::CreateFloat("HoriSpeed", Greater, -0.1));
	}

	///*** Game Object ***///

	GameObject* mainCamera = CreateEmpty();
	{
		Camera* camera = mainCamera->AddComponent<Camera>();
		scene->camera = camera->main = camera;

		mainCamera->AddComponent<CameraController>();
	}

	//{
	//	GameObject* ImageObject = CreateEmpty();
	//	ImageObject->AddComponent<Image>();
	//	auto mesh = ImageObject->AddComponent<MeshFilter>()->mesh = geometries["Image"].get();;
	//	ImageObject->AddComponent<Renderer>()->materials.push_back(5);
	//	renderObjectsLayer[(int)RenderLayer::UI][mesh].gameObjects.push_back(ImageObject);
	//}
	//
	//{
	//	GameObject* ImageObject = CreateEmpty();
	//	auto img = ImageObject->AddComponent<Image>();
	//	{
	//		img->anchorMin = { 0, 0 };
	//		img->pivot = { 0, 0 };
	//		img->posX = 10;
	//		img->posY = 10;
	//		img->width = 400;
	//		img->height = 40;
	//	}
	//	auto mesh = ImageObject->AddComponent<MeshFilter>()->mesh = geometries["Image"].get();;
	//	ImageObject->AddComponent<Renderer>()->materials.push_back(5);
	//	renderObjectsLayer[(int)RenderLayer::UI][mesh].gameObjects.push_back(ImageObject);
	//}
	//
	//{
	//	GameObject* ImageObject = CreateEmpty();
	//	auto img = ImageObject->AddComponent<Image>();
	//	{
	//		img->anchorMin = { 0, 1 };
	//		img->pivot = { 0, 1 };
	//		img->posX = 10;
	//		img->posY = -10;
	//		img->width = 80;
	//		img->height = 320;
	//	}
	//	auto mesh = ImageObject->AddComponent<MeshFilter>()->mesh = geometries["Image"].get();;
	//	ImageObject->AddComponent<Renderer>()->materials.push_back(5);
	//	renderObjectsLayer[(int)RenderLayer::UI][mesh].gameObjects.push_back(ImageObject);
	//}

	{
		auto ritem = CreateEmpty();
		ritem->GetComponent<Transform>()->Scale({ 5000.0f, 5000.0f, 5000.0f });
		auto mesh = ritem->AddComponent<MeshFilter>()->mesh = geometries["Sphere"].get();
		auto renderer = ritem->AddComponent<Renderer>();
		for (auto& sm : mesh->DrawArgs)
			renderer->materials.push_back(5);
	
		renderObjectsLayer[(int)RenderLayer::Sky][mesh].gameObjects.push_back(ritem);
	}

	std::string name[9] {
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
	int count = 0;
	float interval = 7.0f;
	for (int x = -count; x <= count; ++x)
		for (int z = -count; z <= count; ++z)
		{
			auto ritem = CreateEmpty();
			ritem->GetComponent<Transform>()->Scale({ 0.02, 0.02, 0.02 });
			ritem->GetComponent<Transform>()->Rotate({ 1, 0, 0 }, -90);
			ritem->GetComponent<Transform>()->position = { interval * x, 0.0f, interval * z };
			auto mesh = ritem->AddComponent<SkinnedMeshRenderer>()->mesh = geometries["ApprenticeSK"].get();
			auto renderer = ritem->GetComponent<SkinnedMeshRenderer>();
			for (auto& sm : mesh->DrawArgs)
				renderer->materials.push_back(1);
	
			auto anim = ritem->AddComponent<Animator>();
			anim->controller = controller;
			anim->state = &controller->states[name[i++]];
			anim->TimePos = Random::Range(0.0f, anim->controller->GetClipEndTime(anim->state));
	
			if (!x && !z)
			{
				anim->state = &controller->states["Idle"];
				anim->TimePos = 0;
				auto ref = ritem->AddComponent<CharacterController>();
			}
	
			renderObjectsLayer[(int)RenderLayer::SkinnedOpaque][mesh].gameObjects.push_back(ritem);
		}
	
	
	//{
	//	GameObject* ImageObject = CreateImage();
	//	{
	//		ImageObject->AddComponent<Button>()->AddEvent(
	//			[](void*) {
	//				Debug::Log("이게 되네;;\n");
	//				//SceneManager::LoadScene("materialScene");
	//			});
	//	}
	//	{
	//		GameObject* textobject = ImageObject->AddChildUI();
	//		auto rectTransform = textobject->GetComponent<RectTransform>();
	//		rectTransform->anchorMin = { 0, 0 };
	//		rectTransform->anchorMax = { 1, 1 };
	//	
	//		Text* text = textobject->AddComponent<Text>();
	//		text->text = L"되겟냐?ㅋㅋ";
	//		text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
	//		text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
	//		textObjects.push_back(textobject);
	//	}
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
				auto mesh = ritem->AddComponent<MeshFilter>()->mesh = geometries["Cube"].get();
				auto renderer = ritem->AddComponent<Renderer>();
				for (auto& sm : mesh->DrawArgs)
					renderer->materials.push_back(Random::Range(6, 9));
	
				ritem->AddComponent<RotatingBehavior>()->speedRotating = Random::Range(-10.0f, 10.0f) * 2;
	
				renderObjectsLayer[(int)RenderLayer::Opaque][mesh].gameObjects.push_back(ritem);
				
			}
	
	{
		GameObject* grid = CreateEmpty();
		grid->GetComponent<Transform>()->position -= {128, 10, 128};
		auto mesh = grid->AddComponent<MeshFilter>()->mesh = gridMesh;
		grid->AddComponent<Renderer>()->materials.push_back(3);
		renderObjectsLayer[(int)RenderLayer::Opaque][mesh].gameObjects.push_back(grid);
		TerrainPicking* tp = grid->AddComponent<TerrainPicking>();
		tp->terrain = grid;
		tp->prefab = prefab;
		tp->heightMap = m_pHeightMapImage;
		tp->mesh = gridMesh;
	}
	
	for (int i = 0; i < 5; ++i)
	{
		GameObject* leftCylRItem = CreateEmpty();
		leftCylRItem->GetComponent<Transform>()->position = Vector3(-5.0f, 1.5f, -10.0f + i * 5.0f);
		auto mesh = leftCylRItem->AddComponent<MeshFilter>()->mesh = geometries["Cylinder"].get();
		leftCylRItem->AddComponent<Renderer>()->materials.push_back(2);
		renderObjectsLayer[(int)RenderLayer::Opaque][mesh].gameObjects.push_back(leftCylRItem);
	
		GameObject* rightCylRItem = CreateEmpty();
		rightCylRItem->GetComponent<Transform>()->position = Vector3(5.0f, 1.5f, -10.0f + i * 5.0f);
		mesh = rightCylRItem->AddComponent<MeshFilter>()->mesh = geometries["Cylinder"].get();
		rightCylRItem->AddComponent<Renderer>()->materials.push_back(2);
		renderObjectsLayer[(int)RenderLayer::Opaque][mesh].gameObjects.push_back(rightCylRItem);
	
		GameObject* leftSphereRItem = CreateEmpty();
		leftSphereRItem->GetComponent<Transform>()->position = Vector3(-5.0f, 3.5f, -10.0f + i * 5.0f);
		mesh = leftSphereRItem->AddComponent<MeshFilter>()->mesh = geometries["Sphere"].get();
		leftSphereRItem->AddComponent<Renderer>()->materials.push_back(3);
		renderObjectsLayer[(int)RenderLayer::Opaque][mesh].gameObjects.push_back(leftSphereRItem);
	
		GameObject* rightSphereRItem = CreateEmpty();
		rightSphereRItem->GetComponent<Transform>()->position = Vector3(5.0f, 3.5f, -10.0f + i * 5.0f);
		mesh = rightSphereRItem->AddComponent<MeshFilter>()->mesh = geometries["Sphere"].get();
		rightSphereRItem->AddComponent<Renderer>()->materials.push_back(3);
		renderObjectsLayer[(int)RenderLayer::Opaque][mesh].gameObjects.push_back(rightSphereRItem);
	}
}