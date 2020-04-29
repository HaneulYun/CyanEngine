#include "pch.h"
#include "ParticleScene.h"

std::unique_ptr<Mesh> MakeParticle()
{
	auto device = Graphics::Instance()->device;
	auto commandList = Graphics::Instance()->commandList;
	auto mesh = std::make_unique<Mesh>();

	mesh->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	std::vector<FrameResource::ParticleSpriteVertex> vertices;
	FrameResource::ParticleSpriteVertex vertex;
	vertex.Pos = { 0, 0, 0 };
	vertex.Type = 0;
	vertex.LifeTime = 1;
	vertices.emplace_back(vertex);

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(FrameResource::ParticleSpriteVertex);
	D3DCreateBlob(vbByteSize, &mesh->VertexBufferCPU);
	CopyMemory(mesh->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT, 0, 0), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT64) + vbByteSize * 100, D3D12_RESOURCE_FLAG_NONE, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT),
		D3D12_RESOURCE_STATE_STREAM_OUT, nullptr, IID_PPV_ARGS(mesh->VertexBufferGPU.GetAddressOf()));
	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT64) + vbByteSize * 100),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(mesh->VertexBufferUploader.GetAddressOf()));
	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT64) + vbByteSize * 100),
		D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(mesh->VertexBufferReadback.GetAddressOf()));

	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT, 0, 0), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT64) + vbByteSize * 100, D3D12_RESOURCE_FLAG_NONE, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT),
		D3D12_RESOURCE_STATE_STREAM_OUT, nullptr, IID_PPV_ARGS(mesh->VertexStreamBufferGPU.GetAddressOf()));
	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT64) + vbByteSize * 100),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(mesh->VertexStreamBufferUploader.GetAddressOf()));

	char* data = new char[sizeof(UINT64) + vbByteSize];
	memset(data, 0, sizeof(UINT64));
	memcpy(data + sizeof(UINT64), vertices.data(), vbByteSize);
	D3D12_SUBRESOURCE_DATA subResourceData = { data, sizeof(UINT64) + vbByteSize, sizeof(UINT64) + vbByteSize };
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mesh->VertexBufferGPU.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
	UpdateSubresources<1>(commandList.Get(), mesh->VertexBufferGPU.Get(), mesh->VertexBufferUploader.Get(), 0, 0, 1, &subResourceData);
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mesh->VertexBufferGPU.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));


	mesh->VertexByteStride = sizeof(FrameResource::ParticleSpriteVertex);
	mesh->VertexBufferByteSize = vbByteSize;
	mesh->IndexFormat = DXGI_FORMAT_R16_UINT;
	//mesh->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)vertices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	mesh->DrawArgs["submesh"] = submesh;

	return std::move(mesh);
}

void ParticleScene::BuildObjects()
{
	///*** Asset ***///
	//*** Texture ***//
	AddTexture(0, "none", L"Textures\\none.dds");

	//*** Material ***//
	AddMaterial(0, "none", 0);

	//*** Mesh ***//
	geometries["Image"] = Mesh::CreateQuad();
	geometries["Sphere"] = Mesh::CreateSphere();
	geometries["Plane"] = Mesh::CreatePlane();
	geometries["Quad"] = Mesh::CreateQuad();
	geometries["Particle"] =  MakeParticle();

	///*** Game Object ***///

	auto mainCamera = CreateEmpty();
	{
		camera = camera->main = mainCamera->AddComponent<Camera>();
		mainCamera->AddComponent<CameraController>();
	}

	auto skyBox = CreateEmpty();
	{
		skyBox->GetComponent<Transform>()->Scale({ 5000.0f, 5000.0f, 5000.0f });
		skyBox->AddComponent<Renderer>()->materials.push_back(1);
		auto mesh = skyBox->AddComponent<MeshFilter>()->mesh = geometries["Sphere"].get();
		skyBox->layer = (int)RenderLayer::Sky;
	}

	auto menuSceneButton = CreateImage();
	{
		auto rectTransform = menuSceneButton->GetComponent<RectTransform>();
		rectTransform->anchorMin = { 0, 1 };
		rectTransform->anchorMax = { 0, 1 };
		rectTransform->pivot = { 0, 1 };
		rectTransform->posX = 10;
		rectTransform->posY = -10;
		rectTransform->width = 150;
		rectTransform->height = 30;

		menuSceneButton->AddComponent<Button>()->AddEvent(
			[](void*) {
				SceneManager::LoadScene("MenuScene");
			});
		{
			auto textobject = menuSceneButton->AddChildUI();
			auto rectTransform = textobject->GetComponent<RectTransform>();
			rectTransform->anchorMin = { 0, 0 };
			rectTransform->anchorMax = { 1, 1 };

			Text* text = textobject->AddComponent<Text>();
			text->text = L"Menu Scene";
			text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
			text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			textObjects.push_back(textobject);
		}
	}

	auto grid = CreateEmpty();
	{
		grid->AddComponent<MeshFilter>()->mesh = geometries["Plane"].get();
		grid->AddComponent<Renderer>()->materials.push_back(0);
	}

	auto particleSystemObject = CreateEmpty();
	{
		particleSystemObject->AddComponent<ParticleSystem>()->particle = geometries["Particle"].get();
		particleSystemObject->AddComponent<Renderer>()->materials.push_back(0);
		particleSystemObject->layer = (int)RenderLayer::Particle;
		renderObjectsLayer[(int)RenderLayer::Particle][geometries["Particle"].get()].gameObjects.push_back(particleSystemObject);
	}
}
