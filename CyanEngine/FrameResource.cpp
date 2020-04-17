#include "pch.h"
#include "FrameResource.h"

FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT skinnedObjectCount, UINT materialCount, UINT matIndexCount)
{
    ThrowIfFailed(device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));

    PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
	MaterialBuffer = std::make_unique<UploadBuffer<MaterialData>>(device, materialCount, false);
    ObjectCB = std::make_unique<UploadBuffer<InstanceData>>(device, objectCount, false);
	SkinnedCB = std::make_unique<UploadBuffer<SkinnnedData>>(device, skinnedObjectCount, false);
	MatIndexBuffer = std::make_unique<UploadBuffer<MatIndexData>>(device, matIndexCount, false);
}

FrameResource::~FrameResource()
{

}