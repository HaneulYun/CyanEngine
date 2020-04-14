#pragma once

class Scene;
class SkinnedModelInstance;

enum class CollisionType
{
	eCollisionEnter, eCollisionStay, eCollisionExit,
	eTriggerEnter, eTriggerStay, eTriggerExit
};

class GameObject : public Object
{
public:
	Scene* scene{ nullptr };

	GameObject* parent{ nullptr };
	std::deque<Component*> components;
	std::deque<GameObject*> children;
	Transform* transform{ nullptr };
	Component* meshFilter{ nullptr };
	Component* renderer{ nullptr };

	char m_pstrFrameName[64];

	int m_nMaterials = 0;
	CMaterial** m_ppMaterials = NULL;

	std::map<GameObject*, CollisionType> collisionType;

public:
	XMFLOAT4X4 World = MathHelper::Identity4x4();
	XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();

	int NumFramesDirty{ NUM_FRAME_RESOURCES };
	UINT ObjCBIndex{ UINT(-1) };

	Material* Mat{ nullptr };
	MeshGeometry* Geo{ nullptr };

	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	UINT IndexCount{ 0 };
	UINT StartIndexLocation{ 0 };
	int BaseVertexLocation{ 0 };

	UINT SkinnedCBIndex = -1;
	SkinnedModelInstance* SkinnedModelInst = nullptr;

private:
	friend class Scene;
	friend class Object;
	friend class ModelManager;

public:
	GameObject(bool = true);
	GameObject(GameObject* , bool = true);
	~GameObject();

	void Start();
	void Update();

	void OnTriggerEnter(GameObject* other);
	void OnTriggerStay(GameObject* other);
	void OnTriggerExit(GameObject* other);
	void OnCollisionEnter(GameObject* other);
	void OnCollisionStay(GameObject* other);
	void OnCollisionExit(GameObject* other);

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