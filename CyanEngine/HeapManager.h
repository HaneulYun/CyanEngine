#pragma once

class HeapManager
{
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr<ID3D12DescriptorHeap> srvHeap;

public:
	void BuildRtvHeap(ID3D12Device* device, int size);

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetRtv(int index) const;
};
