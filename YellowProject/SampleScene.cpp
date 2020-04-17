#include "pch.h"
#include "SampleScene.h"

Scene* Scene::scene{ nullptr };

std::string mSkinnedModelFilename = "ApprenticeSK";

void SampleScene::BuildObjects()
{
	//*** Asset ***//

	//Material* material_defaultMaterial = new DefaultMaterial();
	
	Mesh* mesh_cube = new Cube();
	Mesh* mesh_grid = new Plane();
	Mesh* mesh_sphere = new Sphere();
	Mesh* mesh_cylinder = new Cylinder();
	
	//graphics->textureData.push_back({ "bricksTex", L"..\\CyanEngine\\Textures\\bricks.dds" });
	//graphics->textureData.push_back({ "stoneTex", L"..\\CyanEngine\\Textures\\stone.dds" });
	//graphics->textureData.push_back({ "tileTex", L"..\\CyanEngine\\Textures\\tile.dds" });
	//graphics->textureData.push_back({ "crateTex", L"..\\CyanEngine\\Textures\\WoodCrate01.dds" });
	//graphics->textureData.push_back({ "defaultTex", L"..\\CyanEngine\\Textures\\white1x1.dds" });
	//
	//graphics->materials["bricks0"] = std::make_unique<Material>();
	//graphics->materials["stone0"] = std::make_unique<Material>();
	//graphics->materials["tile0"] = std::make_unique<Material>();
	//graphics->materials["skullMat"] = std::make_unique<Material>();
	//Material* material_bricks0 = graphics->materials["bricks0"].get();
	//Material* material_stone0 = graphics->materials["stone0"].get();
	//Material* material_tile0 = graphics->materials["tile0"].get();
	//Material* material_skullMat = graphics->materials["skullMat"].get();
	//
	//{
	//	material_bricks0->Name = "bricks0";
	//	material_bricks0->MatCBIndex = 0;
	//	material_bricks0->DiffuseSrvHeapIndex = 0;
	//	material_bricks0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//	material_bricks0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	//	material_bricks0->Roughness = 0.1f;
	//
	//	material_stone0->Name = "stone0";
	//	material_stone0->MatCBIndex = 1;
	//	material_stone0->DiffuseSrvHeapIndex = 1;
	//	material_stone0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//	material_stone0->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	//	material_stone0->Roughness = 0.3f;
	//
	//	material_tile0->Name = "tile0";
	//	material_tile0->MatCBIndex = 2;
	//	material_tile0->DiffuseSrvHeapIndex = 2;
	//	material_tile0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//	material_tile0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	//	material_tile0->Roughness = 0.2f;
	//
	//	material_skullMat->Name = "skullMat";
	//	material_skullMat->MatCBIndex = 3;
	//	material_skullMat->DiffuseSrvHeapIndex = 3;
	//	material_skullMat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//	material_skullMat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05);
	//	material_skullMat->Roughness = 0.3f;
	//}
	for (int i = 0; i < 20; ++i)
	{
		auto material = std::make_unique<Material>();
		material->Name = "material_" + std::to_string(i);
		material->MatCBIndex = i;
		material->DiffuseSrvHeapIndex = 0;
		material->NormalSrvHeapIndex = 0;
		material->DiffuseAlbedo = RANDOM_COLOR;
		material->FresnelR0 = { 0.01f, 0.01f, 0.01f };
		material->Roughness = 0.0f;
		materials[material->Name] = std::move(material);
	}


	//*** Game Object ***//

	GameObject* mainCamera = CreateEmpty();
	{
		Camera* camera = mainCamera->AddComponent<Camera>();
		camera->SetViewport(0, 0, CyanFW::Instance()->GetWidth(), CyanFW::Instance()->GetHeight(), 0.0f, 1.0f);
		camera->SetScissorRect(0, 0, CyanFW::Instance()->GetWidth(), CyanFW::Instance()->GetHeight());
		camera->GenerateProjectionMatrix(0.3f, 10000.0f, CyanFW::Instance()->GetAspectRatio() , XMConvertToDegrees(0.25f * PI));
		camera->GenerateViewMatrix(XMFLOAT3(0.0f, 0.0f, -15.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		camera->main = camera;

		mainCamera->AddComponent<CameraController>();
	}

	UINT objCBIndex = gameObjects.size();

	int count = 1;
	float interval = 5.0f;
	int skinnedIndex = 0;
	//for (int x = -count; x <= count; ++x)
	//	for (int z = -count; z <= count; ++z)
	//	{
	//		auto ritem = CreateEmpty();
	//		ritem->GetComponent<Transform>()->Scale({ 0.02, 0.02, 0.02 });
	//		ritem->GetComponent<Transform>()->Rotate({ 1, 0, 0 }, -90);
	//		ritem->GetComponent<Transform>()->position = { interval * x, 0.0f, interval * z };
	//		auto mesh = ritem->AddComponent<SkinnedMeshRenderer>()->mesh = geometries[mSkinnedModelFilename].get();
	//		auto renderer = ritem->GetComponent<SkinnedMeshRenderer>();
	//		for (auto& sm : mesh->DrawArgs)
	//			renderer->materials.push_back(Random::Range(0, 16));
	//
	//		ritem->TexTransform = MathHelper::Identity4x4();
	//		//ritem->ObjCBIndex = objCBIndex++;
	//
	//		auto anim = ritem->AddComponent<Animator>();
	//		anim->controller = mSkinnedInfo;
	//		anim->FinalTransforms.resize(mSkinnedInfo->BoneCount());
	//		anim->ClipName = "run";
	//		anim->TimePos = Random::Range(0.0f, anim->controller->GetClipEndTime("run"));
	//
	//		renderObjectsLayer[(int)RenderLayer::SkinnedOpaque][mesh].gameObjects.push_back(ritem);
	//	}

	//count = 100;
	//for (int x = -count; x <= count; ++x)
	//	for (int z = -count; z <= count; ++z)
	//	{
	//		auto ritem = CreateEmpty();
	//		ritem->GetComponent<Transform>()->Scale({ 1, 1, 1 });
	//		ritem->GetComponent<Transform>()->position = { interval * x, 0.0f, interval * z };
	//		auto mesh = ritem->AddComponent<MeshFilter>()->mesh = mesh_cube;
	//		auto renderer = ritem->AddComponent<Renderer>();
	//		for (auto& sm : mesh->DrawArgs)
	//			renderer->materials.push_back(Random::Range(0, 16));
	//
	//		ritem->TexTransform = MathHelper::Identity4x4();
	//
	//		renderObjectsLayer[(int)RenderLayer::Opaque][mesh].gameObjects.push_back(ritem);
	//	}

	int xObjects = 15, yObjects = 15, zObjects = 15;
	for (int x = -xObjects; x <= xObjects; x++)
		for (int y = -yObjects; y <= yObjects; y++)
			for (int z = -zObjects; z <= zObjects; z++)
			{
				auto ritem = CreateEmpty();
				ritem->GetComponent<Transform>()->Scale({ 1, 1, 1 });
				ritem->GetComponent<Transform>()->position = { 50.0f * x, 50.0f * y, 50.0f * z };
				auto mesh = ritem->AddComponent<MeshFilter>()->mesh = mesh_cube;
				auto renderer = ritem->AddComponent<Renderer>();
				for (auto& sm : mesh->DrawArgs)
					renderer->materials.push_back(Random::Range(0, 16));

				ritem->TexTransform = MathHelper::Identity4x4();
				ritem->AddComponent<RotatingBehavior>()->speedRotating = Random::Range(-10.0f, 10.0f);

				renderObjectsLayer[(int)RenderLayer::Opaque][mesh].gameObjects.push_back(ritem);
			}

	//{
	//	GameObject* cube = CreateEmpty();
	//	cube->GetComponent<Transform>()->position = Vector3(0, 1, 3);
	//	cube->GetComponent<Transform>()->localScale = Vector3(2, 2, 2);
	//	cube->AddComponent<MeshFilter>()->mesh = mesh_cube;
	//	cube->AddComponent<Renderer>()->materials.push_back(Random::Range(0, 16));
	//	renderObjectsLayer[(int)RenderLayer::Opaque][mesh_cube].gameObjects.push_back(cube);
	//}
	//
	//{
	//	GameObject* grid = CreateEmpty();
	//	grid->AddComponent<MeshFilter>()->mesh = mesh_grid;
	//	grid->AddComponent<Renderer>()->materials.push_back(Random::Range(0, 16));
	//	renderObjectsLayer[(int)RenderLayer::Opaque][mesh_grid].gameObjects.push_back(grid);
	//}
	//
	//for (int i = 0; i < 5; ++i)
	//{
	//	GameObject* leftCylRItem = CreateEmpty();
	//	leftCylRItem->GetComponent<Transform>()->position = Vector3(-5.0f, 1.5f, -10.0f + i * 5.0f);
	//	leftCylRItem->AddComponent<MeshFilter>()->mesh = mesh_cylinder;
	//	leftCylRItem->AddComponent<Renderer>()->materials.push_back(Random::Range(0, 16));// = material_bricks0;
	//	renderObjectsLayer[(int)RenderLayer::Opaque][mesh_cylinder].gameObjects.push_back(leftCylRItem);
	//
	//	GameObject* rightCylRItem = CreateEmpty();
	//	rightCylRItem->GetComponent<Transform>()->position = Vector3(5.0f, 1.5f, -10.0f + i * 5.0f);
	//	rightCylRItem->AddComponent<MeshFilter>()->mesh = mesh_cylinder;
	//	rightCylRItem->AddComponent<Renderer>()->materials.push_back(Random::Range(0, 16));//material = material_bricks0;
	//	renderObjectsLayer[(int)RenderLayer::Opaque][mesh_cylinder].gameObjects.push_back(rightCylRItem);
	//
	//	GameObject* leftSphereRItem = CreateEmpty();
	//	leftSphereRItem->GetComponent<Transform>()->position = Vector3(-5.0f, 3.5f, -10.0f + i * 5.0f);
	//	leftSphereRItem->AddComponent<MeshFilter>()->mesh = mesh_sphere;
	//	leftSphereRItem->AddComponent<Renderer>()->materials.push_back(Random::Range(0, 16));//material = material_tile0;
	//	renderObjectsLayer[(int)RenderLayer::Opaque][mesh_sphere].gameObjects.push_back(leftSphereRItem);
	//
	//	GameObject* rightSphereRItem = CreateEmpty();
	//	rightSphereRItem->GetComponent<Transform>()->position = Vector3(5.0f, 3.5f, -10.0f + i * 5.0f);
	//	rightSphereRItem->AddComponent<MeshFilter>()->mesh = mesh_sphere;
	//	rightSphereRItem->AddComponent<Renderer>()->materials.push_back(Random::Range(0, 16));//material = material_skullMat;
	//	renderObjectsLayer[(int)RenderLayer::Opaque][mesh_sphere].gameObjects.push_back(rightSphereRItem);
	//}
}