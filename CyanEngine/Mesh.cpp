#include "pch.h"
#include "Mesh.h"

void Mesh::Render(UINT nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView)
{
	if (memcmp(&d3dInstancingBufferView, &D3D12_VERTEX_BUFFER_VIEW(), sizeof(D3D12_VERTEX_BUFFER_VIEW)))
	{
		D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[] = { m_d3dVertexBufferView, d3dInstancingBufferView };
		RendererManager::Instance()->commandList->IASetVertexBuffers(m_nSlot, _countof(pVertexBufferViews), pVertexBufferViews);
	}
	else
		RendererManager::Instance()->commandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);

	RendererManager::Instance()->commandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	if (m_pd3dIndexBuffer)
	{
		RendererManager::Instance()->commandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		RendererManager::Instance()->commandList->DrawIndexedInstanced(m_nIndices, nInstances, 0, 0, 0);
	}
	else
	{
		RendererManager::Instance()->commandList->DrawInstanced(m_nVertices, nInstances, m_nOffset, 0);
	}
}

TriangleMesh::TriangleMesh()
{
	m_nVertices = 3;
	m_nStride = sizeof(DiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	DiffusedVertex pVertices[3];
	pVertices[0] = DiffusedVertex(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(Colors::White));
	pVertices[1] = DiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::White));
	pVertices[2] = DiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::White));

	m_pd3dVertexBuffer = CreateBufferResource(pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

Quad::Quad(float fWidth, float fHeight)
{
	m_nVertices = 4;
	m_nStride = sizeof(DiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	DiffusedVertex pVertices[8];
	pVertices[0] = DiffusedVertex(XMFLOAT3(-fWidth / 2, -fHeight / 2, 0.f), XMFLOAT4(Colors::White));
	pVertices[1] = DiffusedVertex(XMFLOAT3(-fWidth / 2, +fHeight / 2, 0.f), XMFLOAT4(Colors::White));
	pVertices[2] = DiffusedVertex(XMFLOAT3(+fWidth / 2, -fHeight / 2, 0.f), XMFLOAT4(Colors::White));
	pVertices[3] = DiffusedVertex(XMFLOAT3(+fWidth / 2, +fHeight / 2, 0.f), XMFLOAT4(Colors::White));

	m_pd3dVertexBuffer = CreateBufferResource(pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CircleLine::CircleLine(float fRadius)
{
	m_nVertices = 31;
	m_nStride = sizeof(DiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;

	DiffusedVertex pVertices[31];

	for (int angle = 0.0f, i = 0; i < 31; angle += 12, ++i) {

		pVertices[i] = DiffusedVertex(XMFLOAT3(fRadius * cos(angle * 3.141592 / 180), fRadius * sin(angle * 3.141592 / 180), 0.f), XMFLOAT4(Colors::White));
	}
	m_pd3dVertexBuffer = CreateBufferResource(pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

Circle::Circle(float fRadius, int slice)
{
	m_nVertices = slice;
	m_nStride = sizeof(DiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	DiffusedVertex* pVertices = new DiffusedVertex[slice];
	for (int i = 0; i < slice / 2; ++i)
	{
		float a = ((float)i) / slice;
		float b = ((float)(slice - i - 1)) / slice;

		float x = cos(a * 2 * PI) * fRadius;
		float y = sin(a * 2 * PI) * fRadius;
		float z = cos(b * 2 * PI) * fRadius;
		float w = sin(b * 2 * PI) * fRadius;
		pVertices[i * 2 + 0] = DiffusedVertex(XMFLOAT3(x, y, 0.f), XMFLOAT4(Colors::White));
		pVertices[i * 2 + 1] = DiffusedVertex(XMFLOAT3(z, w, 0.f), XMFLOAT4(Colors::White));
	}
	if (slice % 2)
		pVertices[slice-1] = DiffusedVertex(XMFLOAT3(cos(((float)(slice / 2)) / slice * 2 * PI) * fRadius / 2, sin(((float)(slice / 2)) / slice * 2 * PI) * fRadius / 2, 0.f), XMFLOAT4(Colors::White));

	m_pd3dVertexBuffer = CreateBufferResource(pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	delete[] pVertices;
}

CubeMeshDiffused::CubeMeshDiffused(float fWidth, float fHeight, float fDepth)
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

	m_pd3dVertexBuffer = CreateBufferResource(pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

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

	m_pd3dIndexBuffer = CreateBufferResource(pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

void MeshIlluminated::CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, int nVertices)
{
	int nPrimitives = nVertices / 3;
	UINT nIndex0, nIndex1, nIndex2;
	for (int i = 0; i < nPrimitives; i++)
	{
		nIndex0 = i * 3 + 0;
		nIndex1 = i * 3 + 1;
		nIndex2 = i * 3 + 2;
		XMFLOAT3 xmf3Edge01 = NS_Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
		XMFLOAT3 xmf3Edge02 = NS_Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
		pxmf3Normals[nIndex0] = pxmf3Normals[nIndex1] = pxmf3Normals[nIndex2] = NS_Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
	}
}

void MeshIlluminated::CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices / 3) : (nVertices / 3);
	XMFLOAT3 xmf3SumOfNormal, xmf3Edge01, xmf3Edge02, xmf3Normal;
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = pnIndices[i * 3 + 0];
			nIndex1 = pnIndices[i * 3 + 1];
			nIndex2 = pnIndices[i * 3 + 2];
			if (pnIndices && ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j)))
			{
				xmf3Edge01 = NS_Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
				xmf3Edge02 = NS_Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
				xmf3Normal = NS_Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, false);
				xmf3SumOfNormal = NS_Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = NS_Vector3::Normalize(xmf3SumOfNormal);
	}
}

void MeshIlluminated::CalculateTriangleStripVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices - 2) : (nVertices - 2);
	XMFLOAT3 xmf3SumOfNormal(0.0f, 0.0f, 0.0f);
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = ((i % 2) == 0) ? (i + 0) : (i + 1);
			if (pnIndices)
				nIndex0 = pnIndices[nIndex0];
			nIndex1 = ((i % 2) == 0) ? (i + 1) : (i + 0);
			if (pnIndices)
				nIndex1 = pnIndices[nIndex1];
			nIndex2 = (pnIndices) ? pnIndices[i + 2] : (i + 2);

			if ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j))
			{
				XMFLOAT3 xmf3Edge01 = NS_Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Edge02 = NS_Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Normal = NS_Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
				xmf3SumOfNormal = NS_Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = NS_Vector3::Normalize(xmf3SumOfNormal);
	}
}

void MeshIlluminated::CalculateVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, int nVertices, UINT* pnIndices, int nIndices)
{
	switch (m_d3dPrimitiveTopology)
	{
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		if (pnIndices)
			CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices, pnIndices, nIndices);
		else
			CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices);
		break;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		CalculateTriangleStripVertexNormals(pxmf3Normals, pxmf3Positions, nVertices, pnIndices, nIndices);
		break;
	default:
		break;
	}
}

CubeMeshIlluminated::CubeMeshIlluminated(float fWidth, float fHeight, float fDepth)
{
	m_nVertices = 25;
	m_nStride = sizeof(IlluminatedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_nIndices = 36;
	UINT pnIndices[36];
	pnIndices[0] = 0; pnIndices[1] = 1; pnIndices[2] = 3;
	pnIndices[3] = 0; pnIndices[4] = 3; pnIndices[5] = 2;

	pnIndices[6] = 4; pnIndices[7] = 6; pnIndices[8] = 5;
	pnIndices[9] = 5; pnIndices[10] = 6; pnIndices[11] = 7;

	pnIndices[12] = 8; pnIndices[13] = 11; pnIndices[14] = 9;
	pnIndices[15] = 9; pnIndices[16] = 11; pnIndices[17] = 10;

	pnIndices[18] = 12; pnIndices[19] = 13; pnIndices[20] = 14;
	pnIndices[21] = 12; pnIndices[22] = 14; pnIndices[23] = 15;

	pnIndices[24] = 16; pnIndices[25] = 17; pnIndices[26] = 19;
	pnIndices[27] = 16; pnIndices[28] = 19; pnIndices[29] = 18;

	pnIndices[30] = 20; pnIndices[31] = 21; pnIndices[32] = 22;
	pnIndices[33] = 21; pnIndices[34] = 23; pnIndices[35] = 22;

	m_pd3dIndexBuffer = CreateBufferResource(pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	XMFLOAT3 pxmf3Positions[8];
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	pxmf3Positions[0] = XMFLOAT3(-fx, +fy, -fz);
	pxmf3Positions[1] = XMFLOAT3(+fx, +fy, -fz);
	pxmf3Positions[2] = XMFLOAT3(+fx, +fy, +fz);
	pxmf3Positions[3] = XMFLOAT3(-fx, +fy, +fz);
	pxmf3Positions[4] = XMFLOAT3(-fx, -fy, -fz);
	pxmf3Positions[5] = XMFLOAT3(+fx, -fy, -fz);
	pxmf3Positions[6] = XMFLOAT3(+fx, -fy, +fz);
	pxmf3Positions[7] = XMFLOAT3(-fx, -fy, +fz);

	XMFLOAT3 pxmf3Normals[6];
	pxmf3Normals[0] = XMFLOAT3( 1.0f, 0.0f, 0.0f);
	pxmf3Normals[1] = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	pxmf3Normals[2] = XMFLOAT3(0.0f,  1.0f, 0.0f);
	pxmf3Normals[3] = XMFLOAT3(0.0f, -1.0f, 0.0f);
	pxmf3Normals[4] = XMFLOAT3(0.0f, 0.0f,  1.0f);
	pxmf3Normals[5] = XMFLOAT3(0.0f, 0.0f, -1.0f);

	IlluminatedVertex pVertices[25];
	pVertices[0] = IlluminatedVertex(pxmf3Positions[1], pxmf3Normals[0]);
	pVertices[1] = IlluminatedVertex(pxmf3Positions[2], pxmf3Normals[0]);
	pVertices[2] = IlluminatedVertex(pxmf3Positions[5], pxmf3Normals[0]);
	pVertices[3] = IlluminatedVertex(pxmf3Positions[6], pxmf3Normals[0]);

	pVertices[4] = IlluminatedVertex(pxmf3Positions[0], pxmf3Normals[1]);
	pVertices[5] = IlluminatedVertex(pxmf3Positions[3], pxmf3Normals[1]);
	pVertices[6] = IlluminatedVertex(pxmf3Positions[4], pxmf3Normals[1]);
	pVertices[7] = IlluminatedVertex(pxmf3Positions[7], pxmf3Normals[1]);

	pVertices[8] = IlluminatedVertex(pxmf3Positions[0], pxmf3Normals[2]);
	pVertices[9] = IlluminatedVertex(pxmf3Positions[1], pxmf3Normals[2]);
	pVertices[10] = IlluminatedVertex(pxmf3Positions[2], pxmf3Normals[2]);
	pVertices[11] = IlluminatedVertex(pxmf3Positions[3], pxmf3Normals[2]);

	pVertices[12] = IlluminatedVertex(pxmf3Positions[4], pxmf3Normals[3]);
	pVertices[13] = IlluminatedVertex(pxmf3Positions[5], pxmf3Normals[3]);
	pVertices[14] = IlluminatedVertex(pxmf3Positions[6], pxmf3Normals[3]);
	pVertices[15] = IlluminatedVertex(pxmf3Positions[7], pxmf3Normals[3]);

	pVertices[16] = IlluminatedVertex(pxmf3Positions[2], pxmf3Normals[4]);
	pVertices[17] = IlluminatedVertex(pxmf3Positions[3], pxmf3Normals[4]);
	pVertices[18] = IlluminatedVertex(pxmf3Positions[6], pxmf3Normals[4]);
	pVertices[19] = IlluminatedVertex(pxmf3Positions[7], pxmf3Normals[4]);

	pVertices[20] = IlluminatedVertex(pxmf3Positions[0], pxmf3Normals[5]);
	pVertices[21] = IlluminatedVertex(pxmf3Positions[1], pxmf3Normals[5]);
	pVertices[22] = IlluminatedVertex(pxmf3Positions[4], pxmf3Normals[5]);
	pVertices[23] = IlluminatedVertex(pxmf3Positions[5], pxmf3Normals[5]);

	m_pd3dVertexBuffer = CreateBufferResource(pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}