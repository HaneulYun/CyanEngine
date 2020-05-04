#pragma once

struct Texture
{
	std::string Name;
	std::wstring Filename;

	UINT Index;

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
};