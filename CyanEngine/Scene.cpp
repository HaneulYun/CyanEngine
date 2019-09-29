#include "pch.h"
#include "Scene.h"

Scene::Scene()
{
	renderer = Renderer::Instance();
}

Scene::~Scene()
{
}

void Scene::OnStart()
{
	renderer->OnStart();

	Renderer::Instance()->m_pd3dCommandList->Reset(Renderer::Instance()->m_pd3dCommandAllocator, NULL);
	BuildObjects();
	Renderer::Instance()->m_pd3dCommandList->Close();

	ID3D12CommandList* ppd3dCommandLists[] = { Renderer::Instance()->m_pd3dCommandList };
	Renderer::Instance()->m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	Renderer::Instance()->WaitForGpuComplete();

	ReleaseUploadBuffers();
}

void Scene::Update()
{
	AnimateObjects();
	renderer->Update();
}

void Scene::Render()
{
	dynamic_cast<Renderer*>(renderer)->PreRender();
	renderer->Render();

	Renderer::Instance()->m_pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->Render(Renderer::Instance()->m_pd3dCommandList);
	}

	dynamic_cast<Renderer*>(renderer)->PostRender();
}

void Scene::OnDestroy()
{
	ReleaseObjects();
	renderer->OnDestroy();
}

void Scene::BuildObjects(ID3D12Device* pd3dDevice)
{
	pd3dDevice = Renderer::Instance()->m_pd3dDevice;

	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	m_nShaders = 1;
	m_ppShaders = new Shader * [m_nShaders];

	Shader* pShader = new Shader();
	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pShader->BuildObjects(pd3dDevice, Renderer::Instance()->m_pd3dCommandList, NULL);

	m_ppShaders[0] = pShader;
}

void Scene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}
}

void Scene::AnimateObjects(float fTimeElapsed)
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(fTimeElapsed);
	}
}

bool Scene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool Scene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void Scene::ReleaseUploadBuffers()
{
	if (m_ppShaders)
	{
		for (int j = 0; j < m_nShaders; j++)
			if (m_ppShaders[j])
				m_ppShaders[j]->ReleaseUploadBuffers();
	}
}

ID3D12RootSignature* Scene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = 0;
	d3dRootSignatureDesc.pParameters = NULL;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)& pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();
	
	return pd3dGraphicsRootSignature;
}

ID3D12RootSignature* Scene::GetGraphicsRootSignature()
{
	return m_pd3dGraphicsRootSignature;
}
