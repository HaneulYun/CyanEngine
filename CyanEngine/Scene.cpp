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

	renderer->m_pd3dCommandList->Reset(renderer->m_pd3dCommandAllocator, NULL);

	BuildObjects();

	Renderer::Instance()->m_pd3dCommandList->Close();
	ID3D12CommandList* ppd3dCommandLists[] = { Renderer::Instance()->m_pd3dCommandList };
	Renderer::Instance()->m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	Renderer::Instance()->WaitForGpuComplete();

	ReleaseUploadBuffers();
}

void Scene::Update()
{
	AnimateObjects(Time::Instance()->GetTimeElapsed());
	renderer->Update();
}

void Scene::Render()
{
	dynamic_cast<Renderer*>(renderer)->PreRender();
	renderer->Render();

	Camera::Instance()->SetViewportsAndScissorRects(Renderer::Instance()->m_pd3dCommandList);
	Renderer::Instance()->m_pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	Camera::Instance()->UpdateShaderVariables(Renderer::Instance()->m_pd3dCommandList);

	for (GameObject* object : gameObjects)
		object->Render(Renderer::Instance()->m_pd3dCommandList);

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

	Quad* pQuadMesh = new Quad(pd3dDevice, Renderer::Instance()->m_pd3dCommandList);
	GameObject* pGameObject = new GameObject();
	pGameObject->SetMesh(pQuadMesh);

	TriangleMesh* pMesh = new TriangleMesh(pd3dDevice, Renderer::Instance()->m_pd3dCommandList);
	RotatingObject* pRotatingObject = new RotatingObject();
	pRotatingObject->SetMesh(pMesh);

	CubeMeshDiffused* pCubeMesh = new CubeMeshDiffused(pd3dDevice, Renderer::Instance()->m_pd3dCommandList, 1, 1, 1);
	RotatingObject* pRotatingObject2 = new RotatingObject();
	pRotatingObject2->SetMesh(pCubeMesh);

	CDiffusedShader* pShader = new CDiffusedShader();
	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, Renderer::Instance()->m_pd3dCommandList);
	pGameObject->SetShader(pShader);
	pRotatingObject->SetShader(pShader);
	pRotatingObject2->SetShader(pShader);

	gameObjects.push_back(pGameObject);
	gameObjects.push_back(pRotatingObject);
	gameObjects.push_back(pRotatingObject2);
}

void Scene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	
	for (GameObject* object : gameObjects)
	{
		// 제거를 어떻게 안전하게 진행하는지 기억나지 않는다.
		delete object;
	}
}

void Scene::AnimateObjects(float fTimeElapsed)
{
	for (GameObject* object : gameObjects)
		object->Animate(fTimeElapsed);
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
	for (GameObject* object : gameObjects)
		object->ReleaseUploadBuffers();
}

ID3D12RootSignature* Scene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[2];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 16;
	pd3dRootParameters[0].Constants.ShaderRegister = 0;
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 1;
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

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
