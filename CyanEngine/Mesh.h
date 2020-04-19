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
	std::vector<XMFLOAT4X4> BoneOffsets;
	std::vector<int> ParentIndexer;

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
	//virtual void Render(UINT nInstances = 1);

	enum class GeometryType {
		GeometryType_Cube, GeometryType_Sphere,
		GeometryType_Cylinder, GeometryType_Plane
	};
	void Create(GeometryType type);
	static std::unique_ptr<Mesh> CreateCube()
	{
		auto v = std::make_unique<Mesh>();
		v->Create(GeometryType::GeometryType_Cube);
		return std::move(v);
	}
	static std::unique_ptr<Mesh> CreateSphere()
	{
		auto v = std::make_unique<Mesh>();
		v->Create(GeometryType::GeometryType_Sphere);
		return std::move(v);
	}
	static std::unique_ptr<Mesh> CreateCylinder()
	{
		auto v = std::make_unique<Mesh>();
		v->Create(GeometryType::GeometryType_Cylinder);
		return std::move(v);
	}
	static std::unique_ptr<Mesh> CreatePlane()
	{
		auto v = std::make_unique<Mesh>();
		v->Create(GeometryType::GeometryType_Plane);
		return std::move(v);
	}
};