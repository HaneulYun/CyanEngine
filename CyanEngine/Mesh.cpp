#include "pch.h"
#include "Mesh.h"

Mesh::Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pd3dIndexUploadBuffer)
		m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;
}

Mesh::~Mesh()
{
	if (m_pd3dVertexBuffer)
		m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer)
		m_pd3dVertexUploadBuffer->Release();

	if (m_pd3dIndexBuffer)
		m_pd3dIndexBuffer->Release();
	if (m_pd3dIndexUploadBuffer)
		m_pd3dIndexUploadBuffer->Release();
}

void Mesh::ReleaseUploadBuffers()
{
	if (m_pd3dVertexUploadBuffer)
		m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;

};

void Mesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView)
{
	if (memcmp(&d3dInstancingBufferView, &D3D12_VERTEX_BUFFER_VIEW(), sizeof(D3D12_VERTEX_BUFFER_VIEW)))
	{
		D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[] = { m_d3dVertexBufferView, d3dInstancingBufferView };
		pd3dCommandList->IASetVertexBuffers(m_nSlot, _countof(pVertexBufferViews), pVertexBufferViews);
	}
	else
		pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);

	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, nInstances, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, nInstances, m_nOffset, 0);
	}
}

TriangleMesh::TriangleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
	: Mesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 3;
	m_nStride = sizeof(DiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	DiffusedVertex pVertices[3];
	pVertices[0] = DiffusedVertex(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(Colors::White));
	pVertices[1] = DiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::White));
	pVertices[2] = DiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::White));

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

Quad::Quad(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
	: Mesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 4;
	m_nStride = sizeof(DiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	DiffusedVertex pVertices[8];
	pVertices[0] = DiffusedVertex(XMFLOAT3(-5.0f, -1.0f, -5.0f), XMFLOAT4(Colors::White));
	pVertices[1] = DiffusedVertex(XMFLOAT3(-5.0f, -1.0f, +5.0f), XMFLOAT4(Colors::White));
	pVertices[2] = DiffusedVertex(XMFLOAT3(+5.0f, -1.0f, -5.0f), XMFLOAT4(Colors::White));
	pVertices[3] = DiffusedVertex(XMFLOAT3(+5.0f, -1.0f, +5.0f), XMFLOAT4(Colors::White));

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CubeMeshDiffused::CubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth)
	: Mesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 8;
	m_nStride = sizeof(DiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	DiffusedVertex pVertices[8];
	pVertices[0] = DiffusedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT4(RANDOM_COLOR));
	pVertices[1] = DiffusedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT4(RANDOM_COLOR));
	pVertices[2] = DiffusedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT4(RANDOM_COLOR));
	pVertices[3] = DiffusedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT4(RANDOM_COLOR));
	pVertices[4] = DiffusedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT4(RANDOM_COLOR));
	pVertices[5] = DiffusedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT4(RANDOM_COLOR));
	pVertices[6] = DiffusedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT4(RANDOM_COLOR));
	pVertices[7] = DiffusedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT4(RANDOM_COLOR));

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;


	m_nIndices = 18;
	UINT pnIndices[18];
	pnIndices[0] = 5; pnIndices[1] = 6; pnIndices[2] = 4;
	pnIndices[3] = 7; pnIndices[4] = 0; pnIndices[5] = 3;
	pnIndices[6] = 1; pnIndices[7] = 2; pnIndices[8] = 2;
	pnIndices[9] = 3; pnIndices[10] = 3; pnIndices[11] = 7;
	pnIndices[12] = 2; pnIndices[13] = 6; pnIndices[14] = 1;
	pnIndices[15] = 5; pnIndices[16] = 0; pnIndices[17] = 4;
	//pnIndices[18] = 1; pnIndices[19] = 6; pnIndices[20] = 5;
	//pnIndices[21] = 2; pnIndices[22] = 6; pnIndices[23] = 1;
	//pnIndices[24] = 2; pnIndices[25] = 7; pnIndices[26] = 6;
	//pnIndices[27] = 3; pnIndices[28] = 7; pnIndices[29] = 2;
	//pnIndices[30] = 6; pnIndices[31] = 4; pnIndices[32] = 5;
	//pnIndices[33] = 7; pnIndices[34] = 4; pnIndices[35] = 6;

	m_pd3dIndexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}