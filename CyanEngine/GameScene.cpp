#include "pch.h"
#include "GameScene.h"


Scene* Scene::scene{ nullptr };

void GameScene::BuildObjects()
{
	scene = this;

	CubeMeshIlluminated* pCubeMesh = new CubeMeshIlluminated(14.0f, 14.0f, 14.0f);

	Material* defaultMaterial = new DefaultMaterial();
	defaultMaterial->shader = new StandardShader();

	GameObject* terrain = CreateEmpty();
	{
		XMFLOAT3 scale(5.0f, 0.5f, 5.0f);
		XMFLOAT4 color(0.0f, 0.2f, 0.0f, 0.0f);
	
		terrain->GetComponent<Transform>()->position.x = -64 * 5 * 2;
		terrain->GetComponent<Transform>()->position.z = -64 * 5 * 2;
		terrain->GetComponent<Transform>()->position.y = -100.f;
		terrain->AddComponent<Terrain>()->LoadTerrain(L"heightMap.raw", 257, 257, 257, 257, scale, color);
		terrain->AddComponent<Renderer>()->material = defaultMaterial;
		terrain->GetComponent<Renderer>()->material->albedo = XMFLOAT4{ 71.f / 255.f, 102.f / 255.f, 0.f, 1 };
	}
	
	//GameObject* cube = CreateEmpty();
	//{
	//	cube->AddComponent<MeshFilter>()->mesh = pCubeMesh;
	//	cube->AddComponent<Renderer>()->material = defaultMaterial;
	//	cube->AddComponent<RotatingBehavior>();
	//	cube->AddComponent<MovingBehavior>();
	//}

	GameObject* cube = CreateEmpty();
	{
		Material* defaultMaterial = new DefaultMaterial();
		defaultMaterial->shader = new TextureShader();

		cube->AddComponent<MeshFilter>()->mesh = pCubeMesh;
		cube->AddComponent<Renderer>()->material = defaultMaterial;
		cube->AddComponent<RotatingBehavior>();
		//cube->AddComponent<MovingBehavior>();
	}

	float interval = 150.f;
	int xObjects = 3, yObjects = 1, zObjects = 3, i = 0;
	for (int x = -xObjects; x <= xObjects; x++)
		for (int y = -yObjects; y <= yObjects; y++)
			for (int z = -zObjects; z <= zObjects; z++)
			{
				GameObject* model = CreateEmpty();
				model->transform->position = Vector3{ interval * x, interval * y, interval * z };
				model->transform->Rotate(XMFLOAT3(0, 1, 0), 90);
	
				GameObject* child = ModelManager::Instance()->LoadGeometryFromFile("Model/Apache.bin");
				model->AddChild(child);
				model->AddComponent<RotatingBehavior>()->speedRotating = Random::Range(-90.f, 90.f);

				child->children[27]->AddComponent<RotatingBehavior>()->speedRotating = 360;
				child->children[7]->AddComponent<RotatingBehavior>()->speedRotating = -720;
			}

	CTexture* ppTextures[TEXTURES];
	ppTextures[0] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	ppTextures[0]->LoadTextureFromFile(L"Texture/Lava(Diffuse).dds", 0);
	ppTextures[1] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	ppTextures[1]->LoadTextureFromFile(L"Texture/Stone01.dds", 0);
	ppTextures[2] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	ppTextures[2]->LoadTextureFromFile(L"Texture/Metal01.dds", 0);
	ppTextures[3] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	ppTextures[3]->LoadTextureFromFile(L"Texture/Metal02.dds", 0);
	ppTextures[4] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	ppTextures[4]->LoadTextureFromFile(L"Texture/Rock01.dds", 0);
	ppTextures[5] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	ppTextures[5]->LoadTextureFromFile(L"Texture/Lava(Emissive).dds", 0);

	GameObject* sceneManager = CreateEmpty();
	{
		sceneManager->AddComponent<SceneManager>()->gameObject = cube;
	}
}