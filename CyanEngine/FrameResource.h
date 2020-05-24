#pragma once

#include "MathHelper.h"

struct PassConstants
{
	Matrix4x4	View;
	Matrix4x4	InvView;
	Matrix4x4	Proj;
	Matrix4x4	InvProj;
	Matrix4x4	ViewProj;
	Matrix4x4	InvViewProj;
	Matrix4x4	ShadowTransform;
    Vector3		EyePosW { 0.0f, 0.0f, 0.0f };
	float		cbPerObjectPad1{ 0.0f };
	Vector2		RenderTargetSize{ 0.0f, 0.0f };
	Vector2		InvRenderTargetSize{ 0.0f, 0.0f };
	float		NearZ{ 0.0f };
	float		FarZ{ 0.0f };
	float		TotalTime{ 0.0f };
	float		DeltaTime{ 0.0f };

	XMFLOAT4	AmbientLight{ 0.0f, 0.0f, 0.0f, 1.0f };
	Light		Lights[MaxLights]{};
};

struct FrameResource
{
	struct Vertex
	{
		Vector3 Pos{};
		Vector3 Normal{};
		Vector2 TexC{};
		Vector3 TangentU{};
	};
	struct SkinnedVertex
	{
		Vector3 Pos{};
		Vector3 Normal{};
		Vector2 TexC{};
		Vector3 TangentU{};
		Vector3 BoneWeights{};
		BYTE BoneIndices[4]{};
	};
	struct ParticleSpriteVertex
	{
		Vector3 Pos{};
		Vector2 Size{};
		float LifeTime{};
		Vector3 Direction{};
		float Speed{};
		UINT Type{};
	};

public:
	FrameResource(ID3D12Device* device, UINT passCount);
	FrameResource(const FrameResource&) = delete;
    FrameResource& operator=(const FrameResource&) = delete;
	~FrameResource() = default;

	ComPtr<ID3D12CommandAllocator> CmdListAlloc{ nullptr };

	std::unique_ptr<UploadBuffer<PassConstants>> PassCB{ nullptr };

    UINT64 Fence = 0;
};