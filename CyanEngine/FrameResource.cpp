#include "pch.h"
#include "FrameResource.h"

FrameResource::FrameResource(ID3D12Device* device, UINT passCount)
{
    ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));

    PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
}