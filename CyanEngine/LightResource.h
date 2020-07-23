#pragma once

struct LightResource
{
	LightResource() = default;
	LightResource(const LightResource&) = delete;
	LightResource& operator=(const LightResource&) = delete;
	~LightResource() = default;

	//std::unique_ptr<UploadBuffer<InstanceData>> InstanceBuffer{ nullptr };
	//std::unique_ptr<UploadBuffer<SkinnnedData>> SkinnedBuffer{ nullptr };
	//std::unique_ptr<UploadBuffer<MatIndexData>> MatIndexBuffer{ nullptr };
};
