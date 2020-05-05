#include "pch.h"
#include "ParticleSystem.h"

void ParticleSystem::Start()
{
	gameObject->scene->objectRenderManager.AddGameObject(gameObject, gameObject->layer);
}

void ParticleSystem::Set()
{
	auto device = Graphics::Instance()->device;
	auto commandList = Graphics::Instance()->commandList;
	auto mesh = new ParticleBundle();

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

	particle = mesh;
}
