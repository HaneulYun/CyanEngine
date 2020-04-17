#pragma once

struct SubmeshGeometry
{
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;

	UINT MatIndex = 0;

	//DirectX::BoundingBox Bounds;
};

class Mesh
{
public:
	std::string Name{ "Mesh" };

	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU{ nullptr };
	Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU{ nullptr };

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU{ nullptr };

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader{ nullptr };

	UINT VertexByteStride{ 0 };
	UINT VertexBufferByteSize{ 0 };
	DXGI_FORMAT IndexFormat{ DXGI_FORMAT_R16_UINT };
	UINT IndexBufferByteSize{ 0 };

	std::unordered_map<std::string, SubmeshGeometry> DrawArgs;

public:
	Mesh() = default;
	virtual ~Mesh() = default;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VertexByteStride;
		vbv.SizeInBytes = VertexBufferByteSize;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = IndexBufferByteSize;

		return ibv;
	}

	void DisposeUploaders()
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}

public:
	ComPtr<ID3D12Resource> vertexBuffer{ nullptr };
	ComPtr<ID3D12Resource> vertexUploadBuffer{ nullptr };
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	UINT m_nSlot = 0;
	UINT m_nVertices = 0;
	UINT m_nStride = 0;
	UINT m_nOffset = 0;
	UINT m_nType = 0;

	ComPtr<ID3D12Resource> indexBuffer{ nullptr };
	ComPtr<ID3D12Resource> indexUploadBuffer{ nullptr };
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	UINT m_nIndices = 0;
	UINT m_nStartIndex = 0;
	int m_nBaseVertex = 0;

public:
	//virtual void Render(UINT nInstances = 1);

	enum class GeometryType {
		GeometryType_Cube, GeometryType_Sphere,
		GeometryType_Cylinder, GeometryType_Plane
	};
	void Create(GeometryType type);
};