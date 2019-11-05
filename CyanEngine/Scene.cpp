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
	BuildObjects();

	for (GameObject* gameObject : gameObjects)
		gameObject->Start();
	rendererManager->Start();;
}

void Scene::Update()
{
	for (GameObject* gameObject : gameObjects)
		gameObject->Update();
	rendererManager->Update();
}

void Scene::Render()
{
	rendererManager->PreRender();

	Camera::Instance()->SetViewportsAndScissorRects(rendererManager->commandList.Get());
	rendererManager->commandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	Camera::Instance()->UpdateShaderVariables(rendererManager->commandList.Get());

	for (GameObject* object : gameObjects)
		if(!object->renderer)
			object->Render(rendererManager->commandList.Get());

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
	pd3dDevice = rendererManager->device.Get();
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	GameObject* Cube = new GameObject;

	CubeMeshDiffused* pBigCubeMesh = new CubeMeshDiffused(pd3dDevice, rendererManager->commandList.Get(), 12.0f, 12.0f, 12.0f);
	
	PlayerShader* pShader = new PlayerShader();
	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	Material* defaultMaterial = new DefaultMaterial();
	defaultMaterial->shader = new StandardShader();
	defaultMaterial->rootSignature = m_pd3dGraphicsRootSignature;

	MeshFilter* meshFilter = Cube->AddComponent<MeshFilter>();
	meshFilter->mesh = pBigCubeMesh;
	Cube->SetMesh(pBigCubeMesh);
	
	Renderer* renderer = Cube->AddComponent<Renderer>();
	renderer->material = defaultMaterial;
	Cube->SetShader(pShader);
	
	RotatingBehavior* rotatingBehavior = Cube->AddComponent<RotatingBehavior>();
	rotatingBehavior->pos = XMFLOAT3{ 0, 0, 0 };
	rotatingBehavior->speedRotating = 45;

	gameObjects.push_back(Cube);

	//Quad* pQuad = new Quad(pd3dDevice, RendererManager::Instance()->commandList.Get());
	//TriangleMesh* pTriangleMesh = new TriangleMesh(pd3dDevice, rendererManager->commandList.Get());
	//CubeMeshDiffused* pSmallCubeMesh = new CubeMeshDiffused(pd3dDevice, rendererManager->commandList.Get(), 1, 1, 1);
	//CubeMeshDiffused* pBigCubeMesh = new CubeMeshDiffused(pd3dDevice, rendererManager->commandList.Get(), 12.0f, 12.0f, 12.0f);
	//
	//PlayerShader* pShader = new PlayerShader();
	//pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	//
	//Material* defaultMaterial = new DefaultMaterial();
	//defaultMaterial->shader = new StandardShader();
	//defaultMaterial->rootSignature = m_pd3dGraphicsRootSignature;
	//
	//GameObject* pQuadObject = new GameObject();
	//pQuadObject->SetMesh(pQuad);
	//pQuadObject->SetShader(pShader);
	//
	//GameObject* pTriangleObject = new GameObject();
	////pTriangleObject->Set
	//pTriangleObject->SetMesh(pTriangleMesh);
	//pTriangleObject->SetShader(pShader);
	//
	//{
	//	GameObject* pRotatingObject = new GameObject();
	//	pRotatingObject->SetMesh(pSmallCubeMesh);
	//	pRotatingObject->SetShader(pShader);
	//	pRotatingObject->AddComponent<RotatingBehavior>();
	//
	//	gameObjects.push_back(pRotatingObject);
	//}
	//
	//gameObjects.push_back(pQuadObject);
	//gameObjects.push_back(pTriangleObject);
	//
	//int xObjects = 10, yObjects = 0, zObjects = 10, i = 0;
	//for (int x = -xObjects; x <= xObjects; x++)
	//	for (int y = -yObjects; y <= yObjects; y++)
	//		for (int z = -zObjects; z <= zObjects; z++)
	//		{
	//			if (!x && !y && !z)
	//				continue;
	//
	//			GameObject* pRotatingObject = new GameObject();
	//
	//			MeshFilter* meshFilter = pRotatingObject->AddComponent<MeshFilter>();
	//			meshFilter->mesh = pBigCubeMesh;
	//			pRotatingObject->SetMesh(pBigCubeMesh);
	//
	//			Renderer* renderer = pRotatingObject->AddComponent<Renderer>();
	//			//renderer->materials.push_back(defaultMaterial);
	//			renderer->material = defaultMaterial;
	//			pRotatingObject->SetShader(pShader);
	//
	//			RotatingBehavior* rotatingBehavior = pRotatingObject->AddComponent<RotatingBehavior>();
	//			rotatingBehavior->pos = XMFLOAT3{ 30.0f * x, 30.0f * y, 30.0f * z };
	//			rotatingBehavior->speedRotating = 10.0f * (i++ % 10);
	//
	//			gameObjects.push_back(pRotatingObject);
	//		}
}

void Scene::ReleaseObjects()
{
	for (GameObject* object : gameObjects)
		delete object;
	gameObjects.clear();

	if (m_pd3dGraphicsRootSignature)
		m_pd3dGraphicsRootSignature->Release();
}

bool Scene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool Scene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

ID3D12RootSignature* Scene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER rootParameter[3];
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParameter[0].Constants.Num32BitValues = 16;
	rootParameter[0].Constants.ShaderRegister = 0;
	rootParameter[0].Constants.RegisterSpace = 0;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParameter[1].Constants.Num32BitValues = 32;
	rootParameter[1].Constants.ShaderRegister = 1;
	rootParameter[1].Constants.RegisterSpace = 0;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	rootParameter[2].Descriptor.ShaderRegister = 0;
	rootParameter[2].Descriptor.RegisterSpace = 0;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	::ZeroMemory(&rootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	rootSignatureDesc.NumParameters = _countof(rootParameter);
	rootSignatureDesc.pParameters = rootParameter;
	rootSignatureDesc.NumStaticSamplers = 0;
	rootSignatureDesc.pStaticSamplers = NULL;
	rootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	::D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)& pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();
	
	return pd3dGraphicsRootSignature;
}

ID3D12RootSignature* Scene::GetGraphicsRootSignature()
{
	return m_pd3dGraphicsRootSignature;
}
