#include "pch.h"
#include "GameObject.h"

GameObject::GameObject()
{
	//XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
	Start();
}
GameObject::~GameObject()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}

void GameObject::Start()
{
	transform = new Transform();
	renderer = new Renderer();

	for (Component* component : components)
		component->Start();
}

void GameObject::Update()
{
	for (Component* component : components)
		component->Update();
}

void GameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList) 
{
	UpdateShaderVariables(pd3dCommandList);

	if (m_pShader)
	{
		m_pShader->UpdateShaderVariable(pd3dCommandList, &transform->localToWorldMatrix);
		m_pShader->Render(pd3dCommandList, Camera::Instance());
	}

	if (m_pMesh) m_pMesh->Render(pd3dCommandList);
}

void GameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera, UINT nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView)
{
	if (m_pMesh)
	{
		if(memcmp(&d3dInstancingBufferView, &D3D12_VERTEX_BUFFER_VIEW(), sizeof(D3D12_VERTEX_BUFFER_VIEW)))
			m_pMesh->Render(pd3dCommandList, nInstances, d3dInstancingBufferView);
		else
			m_pMesh->Render(pd3dCommandList, nInstances);
	}
}

void GameObject::Destroy()
{
}

void GameObject::SetShader(Shader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}
void GameObject::SetMesh(Mesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

//void GameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
//{
//}

void GameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&transform->localToWorldMatrix)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

//void GameObject::ReleaseShaderVariables()
//{
//}

//void GameObject::ReleaseUploadBuffers()
//{
//	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
//}