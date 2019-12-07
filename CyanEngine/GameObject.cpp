#include "pch.h"
#include "GameObject.h"

GameObject::GameObject(bool addition)
{
	scene = Scene::scene;

	transform = new Transform();
	components.push_back(transform);

	if (addition && scene)
		scene->AddGameObject(this);
}

GameObject::GameObject(GameObject* original, bool addition)
{
	scene = Scene::scene;

	for (GameObject* child : original->children)
		AddChild(new GameObject(child));
	for (Component* component : original->components)
		AddComponent(component);

	if (addition && scene)
		scene->AddGameObject(this);
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

void GameObject::OnTriggerEnter(GameObject* other)
{
	for (Component* component : components)
		component->OnTriggerEnter(other);
}

void GameObject::OnTriggerStay(GameObject* other)
{
	for (Component* component : components)
		component->OnTriggerStay(other);
}

void GameObject::OnTriggerExit(GameObject* other)
{
	for (Component* component : components)
		component->OnTriggerExit(other);
}

void GameObject::OnCollisionEnter(GameObject* other)
{
	for (Component* component : components)
		component->OnCollisionEnter(other);
}

void GameObject::OnCollisionStay(GameObject* other)
{
	for (Component* component : components)
		component->OnCollisionStay(other);
}

void GameObject::OnCollisionExit(GameObject* other)
{
	for (Component* component : components)
		component->OnCollisionExit(other);
}

XMFLOAT4X4 GameObject::GetMatrix()
{
	if (parent)
		return NS_Matrix4x4::Multiply(transform->localToWorldMatrix, parent->GetMatrix());
	return transform->localToWorldMatrix;
}
