#pragma once

class Scene;
class RenderSets;

enum class CollisionType
{
	eCollisionEnter, eCollisionStay, eCollisionExit,
	eTriggerEnter, eTriggerStay, eTriggerExit
};

class GameObject : public Object
{
public:
	bool active{ true };
	RenderSets* renderSet{ nullptr };

public:
	Scene* scene{ nullptr };
	int layer{ 0 };
	int instanceIndex{ -1 };

	GameObject* parent{ nullptr };
	std::deque<GameObject*> children;

	std::deque<Component*> components;
	Transform* transform{ nullptr };
	Component* meshFilter{ nullptr };
	Component* renderer{ nullptr };

	std::map<GameObject*, CollisionType> collisionType;

public:
	Matrix4x4 World;
	Matrix4x4 TexTransform;

	int NumFramesDirty{ -1 };

private:
	friend class Scene;
	friend class Object;
	friend class ModelManager;

public:
	GameObject(bool isUI);
	GameObject(GameObject*);
	~GameObject() {}

	void Start();
	void Update();

	void OnTriggerEnter(GameObject* other);
	void OnTriggerStay(GameObject* other);
	void OnTriggerExit(GameObject* other);
	void OnCollisionEnter(GameObject* other);
	void OnCollisionStay(GameObject* other);
	void OnCollisionExit(GameObject* other);

	Matrix4x4 GetMatrix(GameObject* local = nullptr);

	void SetScene(Scene* scene);
	void SetActive(bool state);

	void ReleaseRenderSet();

	GameObject* AddChild(GameObject* child = nullptr)
	{
		if (!child)
			child = new GameObject(false);
		child->scene = scene;
		child->parent = this;
		children.push_back(child);
		return child;
	}
	GameObject* AddChildUI(GameObject* child = nullptr)
	{
		if (!child)
			child = new GameObject(true);
		child->scene = scene;
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
	template <typename T>
	T* GetComponentInChildren();

	GameObject* GetChildWithName(std::string name)
	{
		if (this->name == name)
			return this;
		else
			for (auto child : children)
				if (auto c = child->GetChildWithName(name); c)
					return c;
		return nullptr;
	}
	
	GameObject* GetChildWithIndex(int& index)
	{
		if (!index)
			return this;
		else
			for (auto child : children)
				if (auto c = child->GetChildWithIndex(--index); c)
					return c;
		return nullptr;
	}

	int GetChildCount()
	{
		int count = 1;
		for (auto child : children)
			count += child->GetChildCount();
		return count;
	}
};

template<typename T>
T* GameObject::AddComponent(T* _component)
{
	std::string str = typeid(T).name();

	Component* component = _component->Duplicate(_component);

	components.push_back(component);

	if (typeid(Transform).name() == typeid(*_component).name())
		transform = dynamic_cast<Transform*>(component);
	if (typeid(MeshFilter).name() == typeid(*_component).name())
		meshFilter = component;
	if (typeid(Renderer).name() == typeid(*_component).name())
		renderer = component;
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
		else if (typeid(Collider).name() == typeid(T).name())
		{
			if (typeid(*component).name() == typeid(BoxCollider).name() ||
				typeid(*component).name() == typeid(SphereCollider).name())
				return dynamic_cast<T*>(component);
		}
	return nullptr;
}

template<typename T>
T* GameObject::GetComponentInChildren()
{
	for (auto child : children)
		if (auto component = child->GetComponent<T>(); component)
			return component;
	return nullptr;
}