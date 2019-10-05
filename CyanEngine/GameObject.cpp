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

	for (Component* component : components)
		component->Start();
}

void GameObject::Update()
{
	for (Component* component : components)
		component->Update();
}

void GameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera, UINT nInstances)
{
	OnPrepareRender();

	if (m_pMesh)
		m_pMesh->Render(pd3dCommandList, nInstances);
}

void GameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera, UINT nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView)
{
	OnPrepareRender();
	
	if (m_pMesh)
		m_pMesh->Render(pd3dCommandList, nInstances, d3dInstancingBufferView);
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

void GameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
}

void GameObject::Animate(float fTimeElapsed)
{
}

void GameObject::OnPrepareRender()
{
}

void GameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList) 
{
	OnPrepareRender();

	UpdateShaderVariables(pd3dCommandList);

	if (m_pShader)
	{
		m_pShader->UpdateShaderVariable(pd3dCommandList, &transform->localToWorldMatrix);
		m_pShader->Render(pd3dCommandList, Camera::Instance());
	}

	if (m_pMesh) m_pMesh->Render(pd3dCommandList);
}

void GameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	transform->localToWorldMatrix = Matrix4x4::Multiply(mtxRotate, transform->localToWorldMatrix);
}

RotatingObject::RotatingObject()
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 90.0f;
}

RotatingObject::~RotatingObject()
{
}

void RotatingObject::Animate(float fTimeElapsed)
{
	GameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
}

void GameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void GameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&transform->localToWorldMatrix)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

void GameObject::ReleaseShaderVariables()
{
}

void GameObject::SetPosition(float x, float y, float z)
{
	transform->localToWorldMatrix._41 = x;
	transform->localToWorldMatrix._42 = y;
	transform->localToWorldMatrix._43 = z;
}