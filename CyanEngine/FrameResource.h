#pragma once

#include "MathHelper.h"

struct InstanceData
{
	XMFLOAT4X4 World{ MathHelper::Identity4x4() };
	XMFLOAT4X4 TexTransform{ MathHelper::Identity4x4() };
	UINT MaterialIndexStride{};
	UINT BoneTransformStride{};
	UINT ObjPad0{};
	UINT ObjPad1{};
};

struct MatIndexData
{
	UINT MaterialIndex{};
};

struct SkinnnedData
{
	XMFLOAT4X4 BoneTransforms{};
};


struct PassConstants
{
	XMFLOAT4X4	View{ MathHelper::Identity4x4() };
	XMFLOAT4X4	InvView{ MathHelper::Identity4x4() };
	XMFLOAT4X4	Proj{ MathHelper::Identity4x4() };
	XMFLOAT4X4	InvProj{ MathHelper::Identity4x4() };
	XMFLOAT4X4	ViewProj{ MathHelper::Identity4x4() };
	XMFLOAT4X4	InvViewProj{ MathHelper::Identity4x4() };
    XMFLOAT3	EyePosW { 0.0f, 0.0f, 0.0f };
	float		cbPerObjectPad1{ 0.0f };
	XMFLOAT2	RenderTargetSize{ 0.0f, 0.0f };
	XMFLOAT2	InvRenderTargetSize{ 0.0f, 0.0f };
	float		NearZ{ 0.0f };
	float		FarZ{ 0.0f };
	float		TotalTime{ 0.0f };
	float		DeltaTime{ 0.0f };

	XMFLOAT4	AmbientLight{ 0.0f, 0.0f, 0.0f, 1.0f };
	Light		Lights[MaxLights]{};
};

struct MaterialData
{
	XMFLOAT4 DiffuseAlbedo{ 1.0f, 1.0f, 1.0f, 1.0f };
	XMFLOAT3 FresnelR0{ 0.01f, 0.01f, 0.01f };
	float Roughness{ 0.5f };

	XMFLOAT4X4 MatTransform{ MathHelper::Identity4x4() };

	UINT DiffuseMapIndex{};
	UINT NormalMapIndex{};
	UINT MaterialPad1{};
	UINT MaterialPad2{};
};

struct FrameResource
{
	struct Vertex
	{
		XMFLOAT3 Pos{};
		XMFLOAT3 Normal{};
		XMFLOAT2 TexC{};
		XMFLOAT3 TangentU{};
	};
	struct SkinnedVertex
	{
		XMFLOAT3 Pos{};
		XMFLOAT3 Normal{};
		XMFLOAT2 TexC{};
		XMFLOAT3 TangentU{};
		XMFLOAT3 BoneWeights{};
		BYTE BoneIndices[4]{};
	};
public:
    
	FrameResource(ID3D12Device* device, UINT passCount, UINT materialCount);
	FrameResource(const FrameResource&) = delete;
    FrameResource& operator=(const FrameResource&) = delete;
	~FrameResource() = default;

	ComPtr<ID3D12CommandAllocator> CmdListAlloc{ nullptr };

	std::unique_ptr<UploadBuffer<PassConstants>> PassCB{ nullptr };
	std::unique_ptr<UploadBuffer<MaterialData>> MaterialBuffer{ nullptr };

    UINT64 Fence = 0;
};