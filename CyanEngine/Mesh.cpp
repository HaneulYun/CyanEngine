#include "pch.h"
#include "Mesh.h"

Mesh::Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

Mesh::~Mesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
}

void Mesh::ReleaseUploadBuffers()
{
	//���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��.
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;
};

void Mesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}

TriangleMesh::TriangleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
	: Mesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 3;
	m_nStride = sizeof(DiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	DiffusedVertex pVertices[3];
	pVertices[0] = DiffusedVertex(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = DiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	pVertices[2] = DiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Blue));

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
	pVertices[0] = DiffusedVertex(XMFLOAT3(-5.0f, -1.0f, -5.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = DiffusedVertex(XMFLOAT3(-5.0f, -1.0f, +5.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	pVertices[2] = DiffusedVertex(XMFLOAT3(+5.0f, -1.0f, -5.0f), XMFLOAT4(Colors::Cyan));
	pVertices[3] = DiffusedVertex(XMFLOAT3(+5.0f, -1.0f, +5.0f), XMFLOAT4(Colors::Blue));

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CubeMeshDiffused::CubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth)
	: Mesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 36;
	m_nStride = sizeof(DiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	DiffusedVertex pVertices[36];
	int i = 0;


	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);

	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);

	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);

	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);

	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);

	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);

	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);

	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);

	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);

	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);

	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);

	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CubeMeshDiffused::~CubeMeshDiffused()
{
}
