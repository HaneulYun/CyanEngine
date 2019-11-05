#include "pch.h"
#include "Scene.h"

Scene::Scene()
{
	rendererManager = RendererManager::Instance();
}

Scene::~Scene()
{
}

void Scene::Start()
{
	rendererManager->Start();

	rendererManager->commandList->Reset(rendererManager->commandAllocator.Get(), NULL);

	BuildObjects();
	for (GameObject* gameObject : gameObjects)
		gameObject->Start();
	rendererManager->Start();

	rendererManager->commandList->Close();
	ID3D12CommandList* ppd3dCommandLists[] = { rendererManager->commandList.Get() };
	rendererManager->commandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	rendererManager->WaitForGpuComplete();

	//ReleaseUploadBuffers();
}

void Scene::Update()
{
	for (GameObject* gameObject : gameObjects)
		gameObject->Update();

	//AnimateObjects(Time::Instance()->GetTimeElapsed());
	rendererManager->Update();
}

void Scene::Render()
{
	rendererManager->PreRender();
	//renderer->Render();

	Camera::Instance()->SetViewportsAndScissorRects(RendererManager::Instance()->commandList.Get());
	RendererManager::Instance()->commandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	Camera::Instance()->UpdateShaderVariables(RendererManager::Instance()->commandList.Get());

	for (GameObject* object : gameObjects)
		if(!object->renderer)
			object->Render(RendererManager::Instance()->commandList.Get());
	

	//for (int i = 0; i < m_nShaders; i++)
	//{
	//	m_pShaders[i].Render(RendererManager::Instance()->m_pd3dCommandList, Camera::Instance());
	//}
	rendererManager->Render();

	rendererManager->PostRender();
}

void Scene::Destroy()
{
	ReleaseObjects();
	rendererManager->Destroy();
}

void Scene::BuildObjects(ID3D12Device* pd3dDevice)
{
	pd3dDevice = RendererManager::Instance()->device.Get();
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	
	Quad* pQuad = new Quad(pd3dDevice, RendererManager::Instance()->commandList.Get());
	TriangleMesh* pTriangleMesh = new TriangleMesh(pd3dDevice, rendererManager->commandList.Get());
	CubeMeshDiffused* pSmallCubeMesh = new CubeMeshDiffused(pd3dDevice, rendererManager->commandList.Get(), 1, 1, 1);
	CubeMeshDiffused* pBigCubeMesh = new CubeMeshDiffused(pd3dDevice, rendererManager->commandList.Get(), 12.0f, 12.0f, 12.0f);
	
	PlayerShader* pShader = new PlayerShader();
	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);

	Material* defaultMaterial = new DefaultMaterial();
	defaultMaterial->shader = new StandardShader();
	defaultMaterial->rootSignature = m_pd3dGraphicsRootSignature;

	GameObject* pQuadObject = new GameObject();
	pQuadObject->SetMesh(pQuad);
	pQuadObject->SetShader(pShader);

	GameObject* pTriangleObject = new GameObject();
	//pTriangleObject->Set
	pTriangleObject->SetMesh(pTriangleMesh);
	pTriangleObject->SetShader(pShader);

	{
		GameObject* pRotatingObject = new GameObject();
		pRotatingObject->SetMesh(pSmallCubeMesh);
		pRotatingObject->SetShader(pShader);
		pRotatingObject->AddComponent<RotatingBehavior>();

		gameObjects.push_back(pRotatingObject);
	}

	gameObjects.push_back(pQuadObject);
	gameObjects.push_back(pTriangleObject);

	int xObjects = 10, yObjects = 0, zObjects = 10, i = 0;
	for (int x = -xObjects; x <= xObjects; x++)
		for (int y = -yObjects; y <= yObjects; y++)
			for (int z = -zObjects; z <= zObjects; z++)
			{
				if (!x && !y && !z)
					continue;

				GameObject* pRotatingObject = new GameObject();

				MeshFilter* meshFilter = pRotatingObject->AddComponent<MeshFilter>();
				meshFilter->mesh = pBigCubeMesh;
				pRotatingObject->SetMesh(pBigCubeMesh);

				Renderer* renderer = pRotatingObject->AddComponent<Renderer>();
				//renderer->materials.push_back(defaultMaterial);
				renderer->material = defaultMaterial;
				pRotatingObject->SetShader(pShader);

				RotatingBehavior* rotatingBehavior = pRotatingObject->AddComponent<RotatingBehavior>();
				rotatingBehavior->pos = XMFLOAT3{ 30.0f * x, 30.0f * y, 30.0f * z };
				rotatingBehavior->speedRotating = 10.0f * (i++ % 10);

				gameObjects.push_back(pRotatingObject);
			}

	//m_nShaders = 1;
	//m_pShaders = new InstancingShader[m_nShaders];
	//m_pShaders[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	//m_pShaders[0].BuildObjects(pd3dDevice, RendererManager::Instance()->m_pd3dCommandList);
}

void Scene::ReleaseObjects()
{
	for (GameObject* object : gameObjects)
	{
		// 제거를 어떻게 안전하게 진행하는지 기억나지 않는다.
		delete object;
	}
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	//for (int i = 0; i < m_nShaders; i++)
	//{
	//	m_pShaders[i].ReleaseShaderVariables();
	//	m_pShaders[i].ReleaseObjects();
	//}
	//if (m_pShaders)
	//	delete[] m_pShaders;
}

//void Scene::AnimateObjects(float fTimeElapsed)
//{
//	//for (GameObject* object : gameObjects)
//	//	object->Animate(fTimeElapsed);
//
//	//for (int i = 0; i < m_nShaders; i++)
//	//{
//	//	m_pShaders[i].AnimateObjects(fTimeElapsed);
//	//}
//}

bool Scene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool Scene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

//void Scene::ReleaseUploadBuffers()
//{
//	//for (GameObject* object : gameObjects)
//	//	object->ReleaseUploadBuffers();
//	//
//	//for (int i = 0; i < m_nShaders; i++)
//	//	m_pShaders[i].ReleaseUploadBuffers();
//}

ID3D12RootSignature* Scene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[3];
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
	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 0; //t0
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

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
