#pragma once
#include "framework.h"

class SceneManager : public MonoBehavior<SceneManager>
{
private:

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
		Material* defaultMaterial = new DefaultMaterial();
		defaultMaterial->shader = new TextureShader();

		gameObject->AddComponent<MeshFilter>()->mesh = new CCubeMeshTextured(14.0f, 14.0f, 14.0f);;
		gameObject->AddComponent<Renderer>()->material = defaultMaterial;
		gameObject->AddComponent<RotatingBehavior>();

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

		defaultMaterial->shader->CreateCbvSrvDescriptorHeaps(0, TEXTURES);
		defaultMaterial->shader->CreateShaderVariables();
		//defaultMaterial->shader->CreateConstantBufferViews(0, m_pd3dcbGameObjects, ncbElementBytes);
		for (int i = 0; i < TEXTURES; i++)
			defaultMaterial->shader->CreateShaderResourceViews(ppTextures[i], 3, false);

		CMaterial* ppMaterials[TEXTURES];
		for (int i = 0; i < TEXTURES; i++)
		{
			ppMaterials[i] = new CMaterial();
			ppMaterials[i]->SetTexture(ppTextures[i]);
		}
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
					instance->GetComponent<Renderer>()->material->albedo = XMFLOAT4(r, g, b, 1.0f);
					instance->GetComponent<RotatingBehavior>()->speedRotating = Random::Range(-90.0f, 90.0f);

					instance->AddComponent<MovingBehavior>();
				}
				time -= boundary;
			}
		}
	}
};

