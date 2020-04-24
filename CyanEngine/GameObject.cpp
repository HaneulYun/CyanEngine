#include "pch.h"
#include "GameObject.h"

GameObject::GameObject(bool isUI)
{
	if(isUI)
		transform = new RectTransform();
	else
		transform = new Transform();
	transform->gameObject = this;
	components.push_back(transform);
}

GameObject::GameObject(GameObject* original)
{
	for (GameObject* child : original->children)
		AddChild(new GameObject(child));

	for (Component* component : original->components)
		AddComponent(component);
}

GameObject::~GameObject()
{
}

void GameObject::Start()
{
	for (Component* component : components)
		component->UpdateComponent();
	for (GameObject* child : children)
		child->Start();
}

void GameObject::Update()
{
	for (Component* component : components)
		component->UpdateComponent();
	for (GameObject* child : children)
		child->Update();
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

Matrix4x4 GameObject::GetMatrix()
{
	if (parent)
		return transform->localToWorldMatrix * parent->GetMatrix();
	return transform->localToWorldMatrix;
}
