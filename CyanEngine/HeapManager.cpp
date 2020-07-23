#include "pch.h"
#include "HeapManager.h"

void HeapManager::BuildRtvHeap(ID3D12Device* device, int size)
{
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.NumDescriptors = size;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&rtvHeap));
}

CD3DX12_CPU_DESCRIPTOR_HANDLE HeapManager::GetRtv(int index) const
{
	auto rtv = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeap->GetCPUDescriptorHandleForHeapStart());
	rtv.Offset(index, Graphics::Instance()->rtvDescriptorSize);
	return rtv;
}
