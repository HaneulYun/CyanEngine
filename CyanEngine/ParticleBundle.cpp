#include "pch.h"
#include "ParticleBundle.h"

ParticleResource::ParticleResource(UINT maxParticles, std::vector<FrameResource::ParticleSpriteVertex>& vertices)
{
	auto device = Graphics::Instance()->device;
	auto commandList = Graphics::Instance()->commandList;

	const UINT vbByteSize = sizeof(FrameResource::ParticleSpriteVertex);

	VertexParticleBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(), commandList.Get(), vertices.data(), vbByteSize, VertexParticleBufferUploader);
}

D3D12_VERTEX_BUFFER_VIEW ParticleBundle::VertexBufferView() const
{
	int index = Scene::scene->frameResourceManager.currFrameResourceIndex;

	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = particleResource[index]->VertexParticleBufferGPU->GetGPUVirtualAddress() + sizeof(UINT64);
	vbv.StrideInBytes = VertexByteStride;
	vbv.SizeInBytes = VertexBufferByteSize * particleResource[index]->particleCount;

	return vbv;
}

D3D12_STREAM_OUTPUT_BUFFER_VIEW ParticleBundle::StreamOutputBufferView() const
{
	int index = (Scene::scene->frameResourceManager.currFrameResourceIndex + 1) % 3;

	D3D12_STREAM_OUTPUT_BUFFER_VIEW sov;
	sov.BufferFilledSizeLocation = particleResource[index]->VertexParticleBufferGPU->GetGPUVirtualAddress();
	sov.BufferLocation = sov.BufferFilledSizeLocation + sizeof(UINT64);
	sov.SizeInBytes = VertexBufferByteSize * maxParticles;

	return sov;
}
