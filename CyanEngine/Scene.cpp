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

	//for (GameObject* object : gameObjects)
	//	if(!object->renderer)
	//		object->Render(rendererManager->commandList.Get());

	rendererManager->Render();
	rendererManager->PostRender();
}

void Scene::Destroy()
{
	ReleaseObjects();
	rendererManager->Destroy();
}

void Scene::BuildObjects(ID3D12Device* _device)
{
	_device = rendererManager->device.Get();


	CubeMeshDiffused* pBigCubeMesh = new CubeMeshDiffused(_device, rendererManager->commandList.Get(), 12.0f, 12.0f, 12.0f);
	

	Material* defaultMaterial = new DefaultMaterial();
	defaultMaterial->shader = new StandardShader();
	
	{
		GameObject* Cube = new GameObject;

		MeshFilter* meshFilter = Cube->AddComponent<MeshFilter>();
		meshFilter->mesh = pBigCubeMesh;

		Renderer* renderer = Cube->AddComponent<Renderer>();
		renderer->material = defaultMaterial;

		RotatingBehavior* rotatingBehavior = Cube->AddComponent<RotatingBehavior>();
		rotatingBehavior->pos = XMFLOAT3{ 0, 0, 0 };
		rotatingBehavior->speedRotating = 45;

		gameObjects.push_back(Cube);
	}

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
}