#pragma once

class ParticleBundle : public Mesh
{
public:
	ComPtr<ID3D12Resource> VertexStreamBufferGPU{ nullptr };
	ComPtr<ID3D12Resource> VertexStreamBufferUploader{ nullptr };

	ComPtr<ID3D12Resource> VertexBufferReadback{ nullptr };

public:
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const
	{
		int i = 0;
		for (auto& submesh : DrawArgs)
			i = submesh.second.IndexCount;
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress() + sizeof(UINT64);
		vbv.StrideInBytes = VertexByteStride;
		vbv.SizeInBytes = VertexBufferByteSize * i;

		return vbv;
	}

	D3D12_STREAM_OUTPUT_BUFFER_VIEW StreamOutputBufferView() const
	{
		D3D12_STREAM_OUTPUT_BUFFER_VIEW sov;
		sov.BufferFilledSizeLocation = VertexStreamBufferGPU->GetGPUVirtualAddress();
		sov.BufferLocation = sov.BufferFilledSizeLocation + sizeof(UINT64);
		sov.SizeInBytes = VertexBufferByteSize * 100;

		return sov;
	}
};

