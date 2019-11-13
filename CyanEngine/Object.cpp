#include "pch.h"
#include "Object.h"

GameObject* Object::Instantiate(GameObject* original)
{
	GameObject* instance = new GameObject(original);
	original->scene->AddGameObject(instance);
	return instance;
}