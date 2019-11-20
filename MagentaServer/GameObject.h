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

private:
	friend class Scene;
	friend class Object;

	GameObject();
	GameObject(GameObject* original);

public:
	~GameObject();

	void Start();
	void Update();
	void Destroy();

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
	{
		transform = dynamic_cast<Transform*>(component);
	}
	if (typeid(MeshFilter).name() == typeid(*_component).name())
	{
		meshFilter = component;
	}
	if (typeid(Renderer).name() == typeid(*_component).name())
	{
		renderer = component;
	}

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
	if (typeid(MeshFilter).name() == typeid(T).name())
		meshFilter = component;
	if (typeid(Renderer).name() == typeid(T).name())
		renderer = component;

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
