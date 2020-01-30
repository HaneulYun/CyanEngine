#pragma once

class Mesh
{
public:
	Mesh() = default;
	virtual ~Mesh() = default;

public:
	ComPtr<ID3D12Resource> vertexBuffer{ nullptr };
	ComPtr<ID3D12Resource> vertexUploadBuffer{ nullptr };
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	UINT m_nSlot = 0;
	UINT m_nVertices = 0;
	UINT m_nStride = 0;
	UINT m_nOffset = 0;
	UINT m_nType = 0;

	ComPtr<ID3D12Resource> indexBuffer{ nullptr };
	ComPtr<ID3D12Resource> indexUploadBuffer{ nullptr };
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	UINT m_nIndices = 0;
	UINT m_nStartIndex = 0;
	int m_nBaseVertex = 0;

public:
	virtual void Render(UINT nInstances = 1);
};