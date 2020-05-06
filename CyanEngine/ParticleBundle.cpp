#include "pch.h"
#include "ParticleBundle.h"

ParticleResource::ParticleResource(UINT maxParticles, D3D12_SUBRESOURCE_DATA subResourceData)
{
	auto device = Graphics::Instance()->device;
	auto commandList = Graphics::Instance()->commandList;

	const UINT vbByteSize = sizeof(FrameResource::ParticleSpriteVertex);

	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT, 0, 0), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT64) + vbByteSize * maxParticles, D3D12_RESOURCE_FLAG_NONE, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT),
		D3D12_RESOURCE_STATE_STREAM_OUT, nullptr, IID_PPV_ARGS(VertexParticleBufferGPU.GetAddressOf()));
	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT64) + vbByteSize * maxParticles),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(VertexParticleBufferUploader.GetAddressOf()));

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(VertexParticleBufferGPU.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
	UpdateSubresources<1>(commandList.Get(), VertexParticleBufferGPU.Get(), VertexParticleBufferUploader.Get(), 0, 0, 1, &subResourceData);
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(VertexParticleBufferGPU.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

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
