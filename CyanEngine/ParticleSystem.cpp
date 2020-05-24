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

	char* data = new char[sizeof(UINT64) + vbByteSize];
	memset(data, 0, sizeof(UINT64));
	memcpy(data + sizeof(UINT64), vertices.data(), vbByteSize);
	D3D12_SUBRESOURCE_DATA subResourceData = { data, sizeof(UINT64) + vbByteSize, sizeof(UINT64) + vbByteSize };

	D3D12_SUBRESOURCE_DATA subInitResourceData = { data, sizeof(UINT64), sizeof(UINT64) };
	//delete data;

	for (int i = 0; i < 3; ++i)
		mesh->particleResource[i] = new ParticleResource(mesh->maxParticles, vertices);

	mesh->VertexParticleInitBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(), commandList.Get(), vertices.data(), vbByteSize, mesh->VertexParticleInitBufferUploader);



	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT64) + vbByteSize * mesh->maxParticles),
		D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(mesh->VertexParticleBufferReadback.GetAddressOf()));

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
