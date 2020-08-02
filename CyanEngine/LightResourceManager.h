#pragma once

class LightData
{
public:
	GameObject* gameObject;
	static std::vector<std::unique_ptr<ShadowMap>> shadowMap;

	std::vector<std::unique_ptr<LightResource>> lightResource;
	int isDirty{ NUM_FRAME_RESOURCES };
};

class LightResourceManager
{
public:
	std::vector<LightData*> lightObjects[Light::Type::Count];

public:
	void AddGameObject(GameObject* gameObject, int layer = 0);

	void Update();
	void Release();
};
