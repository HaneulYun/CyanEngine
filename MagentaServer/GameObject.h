#pragma once

class Scene;
class Component;
class Transform;

class GameObject : public Object
{
public:
	Scene* scene{ nullptr };

	GameObject* parent{ nullptr };
	std::deque<Component*> components;
	std::deque<GameObject*> children;
	Transform* transform{ nullptr };

	char m_pstrFrameName[64];

private:
	friend class Scene;
	friend class Object;

	GameObject();
	GameObject(GameObject* original);

public:
	~GameObject();

	void Start();
	void Update();

	XMFLOAT4X4 GetMatrix();

	GameObject* AddChild(GameObject* child)
	{
		child->parent = this;
		children.push_back(child);
		return child;
	}

	template <typename T>
	T* AddComponent(T* component);
	template <typename T>
	T* AddComponent();
	template <typename T>
	T* GetComponent();
};

template<typename T>
T* GameObject::AddComponent(T* _component)
{
	std::string str = typeid(T).name();

	Component* component = _component->Duplicate(_component);

	components.push_back(component);

	if (typeid(Transform).name() == typeid(*_component).name())
		transform = dynamic_cast<Transform*>(component);

	component->gameObject = this;

	return dynamic_cast<T*>(component);
}

template<typename T>
T* GameObject::AddComponent()
{
	Component* component = new T();
	component->gameObject = this;
	components.push_back(component);

	if (typeid(Transform).name() == typeid(T).name())
		transform = dynamic_cast<Transform*>(component);

	return dynamic_cast<T*>(component);
}

template<typename T>
T* GameObject::GetComponent()
{
	for (Component* component : components)
		if (typeid(*component).name() == typeid(T).name())
			return dynamic_cast<T*>(component);

	return nullptr;
}