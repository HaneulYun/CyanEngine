#include "pch.h"
#include "GameObject.h"

GameObject::GameObject()
{
	transform = new Transform();
	components.push_back(transform);
}

GameObject::GameObject(GameObject* original)
{
	scene = original->scene;

	for (Component* component : original->components)
		AddComponent(component);
}

GameObject::~GameObject()
{
}

void GameObject::Start()
{
	for (GameObject* child : children)
		child->Start();
	for (Component* component : components)
		component->UpdateComponent();
}

void GameObject::Update()
{
	for (GameObject* child : children)
		child->Update();
	for (Component* component : components)
		component->UpdateComponent();
}