#pragma once

class Scene;

class GameObject : public Object
{
public:
	Scene* scene{ nullptr };

	std::deque<Component*> components;
	Transform* transform{ nullptr };
	Component* meshFilter{ nullptr };
	Component* renderer{ nullptr };

public:
	GameObject();
	GameObject(GameObject* original);
	~GameObject();

	void Start();
	void Update();
	void Destroy();

	template <typename T>
	T* AddComponent();
	template <typename T>
	T* GetComponent();
};

template<typename T>
inline T* GameObject::AddComponent()
{
	Component* component = new T();
	component->gameObject = this;
	components.push_back(component);

	if (std::is_same<T, MeshFilter>())
		meshFilter = component;
	if (std::is_same<T, Renderer>())
		renderer = component;

	return dynamic_cast<T*>(component);
}

template<typename T>
inline T* GameObject::GetComponent()
{
	for (Component* component : components)
		//if (typeid(component) == typeid(T))
		if (std::is_same<component, T>::value)
			return component;
	return nullptr;
}
