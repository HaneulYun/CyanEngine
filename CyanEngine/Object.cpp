#include "pch.h"
#include "Object.h"

template <>
GameObject* Object::Instantiate(GameObject* original)
{
	GameObject* instance = new GameObject(original);
	original->scene->AddGameObject(instance);
	return instance;
}

void SceneManager::Start()
{
	int xObjects = 15, yObjects = 15, zObjects = 15, i = 0;
	for (int x = -xObjects; x <= xObjects; x++)
		for (int y = -yObjects; y <= yObjects; y++)
			for (int z = -zObjects; z <= zObjects; z++)
			{
				GameObject* instance = Instantiate(gameObject);

				instance->transform->position = XMFLOAT3{ 10.0f * x, 10.0f * y, 10.0f * z };

				RotatingBehavior* rotatingBehavior = instance->AddComponent<RotatingBehavior>();
				rotatingBehavior->speedRotating = 10.0f * (i++ % 10);
			}
}