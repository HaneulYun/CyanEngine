#pragma once

class GameObject : public Object
{
public:
	std::deque<Component*> components;
	Transform* transform;
	Component* renderer;

public:
	GameObject();
	~GameObject();

	void Start();
	void Update();
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera, UINT nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView = {});

	void Destroy();

	template <typename T>
	void AddComponent();

private:
	//int m_nReferences = 0;

protected:
	Mesh* m_pMesh = NULL;
	Shader* m_pShader = NULL;

public:
	void SetMesh(Mesh* pMesh);
	void SetShader(Shader* pShader);
	//void AddRef() { m_nReferences++; }
	//void Release() { if (--m_nReferences <= 0) delete this; }
	//void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	//void ReleaseShaderVariables();
	//void ReleaseUploadBuffers();
};

template<typename T>
inline void GameObject::AddComponent()
{
	Component* component = new T();
	component->gameObject = this;
	components.push_back(component);
}
