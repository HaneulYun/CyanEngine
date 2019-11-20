#include "pch.h"
#include "Object.h"

GameObject* Object::Instantiate(GameObject* original)
{
	GameObject* instance = original->scene->Duplicate(original);
	return instance;
}

void Object::Destroy(GameObject* obj)
{
	obj->scene->Delete(obj);
}
