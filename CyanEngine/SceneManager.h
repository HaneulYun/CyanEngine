#pragma once
#include "framework.h"

class SceneManager : public MonoBehavior<SceneManager>
{
private:
	TextureShader* shader[6];

public:
	GameObject* gameObject{ nullptr };

private:
	friend class GameObject;
	friend class MonoBehavior<SceneManager>;
	SceneManager() = default;
	SceneManager(SceneManager&) = default;

public:
	~SceneManager() {}

	void Start()
	{
		CTexture* ppTextures[6];
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

		Material* defaultMaterial = new DefaultMaterial();

		gameObject->AddComponent<MeshFilter>()->mesh = new CCubeMeshTextured(14.0f, 14.0f, 14.0f);;
		gameObject->AddComponent<Renderer>()->material = defaultMaterial;
		gameObject->AddComponent<RotatingBehavior>();

		for (int j = 0; j < 6; ++j)
		{
			shader[j] = new TextureShader();
			defaultMaterial->shader = shader[j];

			shader[j]->CreateCbvSrvDescriptorHeaps(0, TEXTURES);
			shader[j]->CreateShaderVariables();
			shader[j]->CreateShaderResourceViews(ppTextures[j], 3, false);

			shader[j]->ppMaterials[0] = new CMaterial();
			shader[j]->ppMaterials[0]->SetTexture(ppTextures[j]);
		}


		//float interval = 150.f;
		//int xObjects = 5, yObjects = 5, zObjects = 5, i = 0;
		//for (int x = -xObjects; x <= xObjects; x++)
		//	for (int y = -yObjects; y <= yObjects; y++)
		//		for (int z = -zObjects; z <= zObjects; z++)
		//		{
		//			GameObject* model = Instantiate(gameObject);
		//			model->transform->position = Vector3{ interval * x, interval * y, interval * z };
		//		}
	}

	void Update()
	{
		static bool instancing = true;
		static float time = 0;
		static const float boundary = 0.0;
		time += Time::deltaTime;

		if (Input::GetMouseButtonDown(0))
			instancing = !instancing;

		if (instancing)
		{
			if (time > boundary)
			{
				for (int i = 0; i < 1; ++i)
				{
					auto instance = Instantiate(gameObject);
					float x = Random::Range(-1500, 1500);
					float y = Random::Range(-1500, 1500);
					float z = Random::Range(-1000, 2000);
					instance->transform->position = Vector3{ x, y, z };
		
					float r = Random::Range(0.0f, 1.0f);
					float g = Random::Range(0.0f, 1.0f);
					float b = Random::Range(0.0f, 1.0f);
					instance->GetComponent<Renderer>()->material->shader = shader[Random::Range(0, 5)];
					instance->GetComponent<RotatingBehavior>()->speedRotating = Random::Range(-90.0f, 90.0f);
		
					instance->AddComponent<MovingBehavior>();
				}
				time -= boundary;
			}
		}
	}
};

