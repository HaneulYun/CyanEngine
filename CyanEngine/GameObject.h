#pragma once

class Scene;

struct MATERIALLOADINFO
{
	XMFLOAT4						m_xmf4AlbedoColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4						m_xmf4EmissiveColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4						m_xmf4SpecularColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	float							m_fGlossiness = 0.0f;
	float							m_fSmoothness = 0.0f;
	float							m_fSpecularHighlight = 0.0f;
	float							m_fMetallic = 0.0f;
	float							m_fGlossyReflection = 0.0f;

	UINT							m_nType = 0x00;

	//char							m_pstrAlbedoMapName[64] = { '\0' };
	//char							m_pstrSpecularMapName[64] = { '\0' };
	//char							m_pstrMetallicMapName[64] = { '\0' };
	//char							m_pstrNormalMapName[64] = { '\0' };
	//char							m_pstrEmissionMapName[64] = { '\0' };
	//char							m_pstrDetailAlbedoMapName[64] = { '\0' };
	//char							m_pstrDetailNormalMapName[64] = { '\0' };
};

struct MATERIALSLOADINFO
{
	int m_nMaterials = 0;
	MATERIALLOADINFO* m_pMaterials = NULL;
};

class CMaterialColors
{
public:
	CMaterialColors() { }
	CMaterialColors(MATERIALLOADINFO* pMaterialInfo) {}
	virtual ~CMaterialColors() { }

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	XMFLOAT4 m_xmf4Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	XMFLOAT4 m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); //(r,g,b,a=power)
	XMFLOAT4 m_xmf4Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
};

class CMaterial
{
public:
	CMaterial() {}
	virtual ~CMaterial() {}

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	Shader* m_pShader = NULL;

	CMaterialColors* m_pMaterialColors = NULL;

	//void SetMaterialColors(CMaterialColors* pMaterialColors);
	//void SetShader(Shader* pShader);
	//void SetIlluminatedShader() { SetShader(m_pIlluminatedShader); }
	
	//void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList);

protected:
	static Shader* m_pIlluminatedShader;

public:
	//static void CMaterial::PrepareShaders(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
};

class GameObject : public Object
{
public:
	Scene* scene{ nullptr };

	std::deque<Component*> components;
	std::deque<GameObject*> children;
	Transform* transform{ nullptr };
	Component* meshFilter{ nullptr };
	Component* renderer{ nullptr };

	char m_pstrFrameName[64];

	int m_nMaterials = 0;
	CMaterial** m_ppMaterials = NULL;

private:
	friend class Scene;
	friend class Object;

	GameObject();
	GameObject(GameObject* original);

public:
	~GameObject();

	void Start();
	void Update();

	template <typename T>
	T* AddComponent(T* component);
	template <typename T>
	T* AddComponent();
	template <typename T>
	T* GetComponent();

	MATERIALSLOADINFO* LoadMaterialsInfoFromFile(FILE* pInFile);
	CMeshLoadInfo* LoadMeshInfoFromFile(FILE* pInFile);

	GameObject* LoadGeometryFromFile(const char* pstrFileName);
	GameObject* LoadFrameHierarchyFromFile(FILE* pInFile);
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
