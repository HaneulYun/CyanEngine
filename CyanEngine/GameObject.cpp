#include "pch.h"
#include "GameObject.h"

GameObject::GameObject()
{
	transform = new Transform();
	components.push_back(transform);
}

GameObject::GameObject(GameObject* original)
{
	for (Component* component : original->components)
		;

	if (original->transform)
	{
		transform = AddComponent<Transform>();
		*(Transform*)transform = *(Transform*)original->transform;
		transform->gameObject = this;
	}
	if (original->meshFilter)
	{
		AddComponent<MeshFilter>();
		*(MeshFilter*)meshFilter = *(MeshFilter*)original->meshFilter;
		meshFilter->gameObject = this;
	}
	if (original->renderer)
	{
		AddComponent<Renderer>();
		*(Renderer*)renderer = *(Renderer*)original->renderer;
		renderer->gameObject = this;
	}
}

GameObject::~GameObject()
{
}

void GameObject::Start()
{
	for (Component* component : components)
		component->Start();
}

void GameObject::Update()
{
	for (Component* component : components)
		component->Update();
}

void GameObject::Destroy()
{
}