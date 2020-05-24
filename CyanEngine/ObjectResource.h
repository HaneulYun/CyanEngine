#pragma once

struct InstanceData
{
	Matrix4x4 World;
	Matrix4x4 TexTransform;
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
	Matrix4x4 BoneTransforms{};
};

struct ObjectsResource
{
	ObjectsResource() = default;
	ObjectsResource(const ObjectsResource&) = delete;
	ObjectsResource& operator=(const ObjectsResource&) = delete;
	~ObjectsResource() = default;

	std::unique_ptr<UploadBuffer<InstanceData>> InstanceBuffer{ nullptr };
	std::unique_ptr<UploadBuffer<SkinnnedData>> SkinnedBuffer{ nullptr };
	std::unique_ptr<UploadBuffer<MatIndexData>> MatIndexBuffer{ nullptr };
};
