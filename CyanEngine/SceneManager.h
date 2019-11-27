#pragma once
#include "framework.h"

class SceneManager : public MonoBehavior<SceneManager>
{
private:
	TextureShader* shader;

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
		CTexture* ppTextures;
		ppTextures = new CTexture(1, RESOURCE_TEXTURE2D, 0);
		ppTextures->LoadTextureFromFile(L"Texture/Lava(Diffuse).dds", 0);

		Material* defaultMaterial = new DefaultMaterial();

		gameObject->AddComponent<MeshFilter>()->mesh = new CCubeMeshTextured(14.0f, 14.0f, 14.0f);;
		gameObject->AddComponent<Renderer>()->material = defaultMaterial;
		gameObject->AddComponent<RotatingBehavior>();

		shader = new TextureShader();
		defaultMaterial->shader = shader;

		shader->CreateCbvSrvDescriptorHeaps(0, TEXTURES);
		//shader->CreateShaderVariables();
		shader->CreateShaderResourceViews(ppTextures, 3, false);

		shader->ppMaterials[0] = new CMaterial();
		shader->ppMaterials[0]->SetTexture(ppTextures);

		for (int i = 0; i < 1; ++i)
		{
			//auto instance = Instantiate(gameObject);
			//instance->transform->position = Vector3{ 0, 100, 10 };
			//
			//instance->GetComponent<Renderer>()->material->shader = shader;
			//instance->GetComponent<RotatingBehavior>()->speedRotating = Random::Range(-90.0f, 90.0f);
			//
			//instance->AddComponent<MovingBehavior>();
		}
	}

	void Update()
	{
		static bool instancing = false;
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
					//auto instance = Instantiate(gameObject);
					//float x = Random::Range(-1500, 1500);
					//float y = Random::Range(-1500, 1500);
					//float z = Random::Range(-1000, 2000);
					//instance->transform->position = Vector3{ x, y, z };
					//
					//float r = Random::Range(0.0f, 1.0f);
					//float g = Random::Range(0.0f, 1.0f);
					//float b = Random::Range(0.0f, 1.0f);
					//instance->GetComponent<Renderer>()->material->shader = shader;
					//instance->GetComponent<RotatingBehavior>()->speedRotating = Random::Range(-90.0f, 90.0f);
					//
					//instance->AddComponent<MovingBehavior>();
				}
				time -= boundary;
			}
		}
	}
};

