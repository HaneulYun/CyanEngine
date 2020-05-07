#pragma once

class ParticleResource
{
public:
	ParticleResource(UINT maxParticles, D3D12_SUBRESOURCE_DATA subResourceData);

	ComPtr<ID3D12Resource> VertexParticleBufferGPU{ nullptr };
	ComPtr<ID3D12Resource> VertexParticleBufferUploader{ nullptr };

	char* data;
	D3D12_SUBRESOURCE_DATA subResourceData;

	int particleCount{ 1 };
};

class ParticleBundle : public Mesh
{
public:
	ParticleResource* particleResource[3];

	ComPtr<ID3D12Resource> VertexParticleBufferReadback{ nullptr };

	int particleCount{ 1 };
	int maxParticles{ 1000 };

public:
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;
	D3D12_STREAM_OUTPUT_BUFFER_VIEW StreamOutputBufferView() const;
};
