#include "pch.h"
#include "Meshs.h"

CHeightMapImage::CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale)
{
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;

	BYTE* pHeightMapPixels = new BYTE[m_nWidth * m_nLength];
	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	::ReadFile(hFile, pHeightMapPixels, (m_nWidth * m_nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	m_pHeightMapPixels = new BYTE[m_nWidth * m_nLength];
	for (int y = 0; y < m_nLength; y++)
		for (int x = 0; x < m_nWidth; x++)
			m_pHeightMapPixels[x + ((m_nLength - 1 - y) * m_nWidth)] = pHeightMapPixels[x + (y * m_nWidth)];

	if (pHeightMapPixels)
		delete[] pHeightMapPixels;
}

CHeightMapImage::~CHeightMapImage()
{
	if (m_pHeightMapPixels)
		delete[] m_pHeightMapPixels;
	m_pHeightMapPixels = NULL;
}

XMFLOAT3 CHeightMapImage::GetHeightMapNormal(int x, int z)
{
	if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength))
		return(XMFLOAT3(0.0f, 1.0f, 0.0f));

	int nHeightMapIndex = x + (z * m_nWidth);
	int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth;

	float y1 = (float)m_pHeightMapPixels[nHeightMapIndex] * m_xmf3Scale.y;
	float y2 = (float)m_pHeightMapPixels[nHeightMapIndex + xHeightMapAdd] * m_xmf3Scale.y;
	float y3 = (float)m_pHeightMapPixels[nHeightMapIndex + zHeightMapAdd] * m_xmf3Scale.y;

	XMFLOAT3 xmf3Edge1 = XMFLOAT3(0.0f, y3 - y1, m_xmf3Scale.z);
	XMFLOAT3 xmf3Edge2 = XMFLOAT3(m_xmf3Scale.x, y2 - y1, 0.0f);
	XMFLOAT3 xmf3Normal = NS_Vector3::CrossProduct(xmf3Edge1, xmf3Edge2, true);

	return(xmf3Normal);
}

float CHeightMapImage::GetHeight(float fx, float fz)
{
	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength))
		return(0.0f);

	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;
	float fBottomLeft = (float)m_pHeightMapPixels[x + (z * m_nWidth)];
	float fBottomRight = (float)m_pHeightMapPixels[(x + 1) + (z * m_nWidth)];
	float fTopLeft = (float)m_pHeightMapPixels[x + ((z + 1) * m_nWidth)];
	float fTopRight = (float)m_pHeightMapPixels[(x + 1) + ((z + 1) * m_nWidth)];

	bool bRightToLeft = ((z % 2) != 0);
	if (bRightToLeft)
	{
		if (fzPercent >= fxPercent)
			fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
		else
			fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}
	else
	{
		if (fzPercent < (1.0f - fxPercent))
			fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
		else
			fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}

	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;

	return(fHeight);
}

CHeightMapGridMesh::CHeightMapGridMesh(int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, void* pContext)
{
	VertexByteStride = sizeof(FrameResource::Vertex);

	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;

#ifdef _WITH_TERRAIN_TESSELATION
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST;
	m_nVertices = 25;
#else
	PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
#endif

	std::vector<FrameResource::Vertex> vertices;
	std::vector<UINT> indices;

	vertices.resize(nWidth * nLength);

	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	int cxHeightMap = pHeightMapImage->GetHeightMapWidth();
	int czHeightMap = pHeightMapImage->GetHeightMapLength();

	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;
#ifdef _WITH_TERRAIN_TESSELATION
	for (int i = 0, z = (zStart + nLength - 1); z >= zStart; z -= 2)
	{
		for (int x = xStart; x < (xStart + nWidth); x += 2, i++)
		{
			fHeight = OnGetHeight(x, z, pContext);
			pVertices[i].position = XMFLOAT3((x * m_xmf3Scale.x), fHeight, (z * m_xmf3Scale.z));
			pVertices[i].color = NS_Vector4::Add(OnGetColor(x, z, pContext), xmf4Color);
			pVertices[i].uv0 = XMFLOAT2(float(x) / float(cxHeightMap - 1), float(czHeightMap - 1 - z) / float(czHeightMap - 1));
			pVertices[i].uv1 = XMFLOAT2(float(x) / float(m_xmf3Scale.x * 0.5f), float(z) / float(m_xmf3Scale.z * 0.5f));
			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
		}
	}
#else
	for (int i = 0, z = zStart; z < (zStart + nLength); z++)
	{
		for (int x = xStart; x < (xStart + nWidth); x++, i++)
		{
			fHeight = OnGetHeight(x, z, pContext);
			vertices[i].Pos = XMFLOAT3((x * m_xmf3Scale.x), fHeight, (z * m_xmf3Scale.z));
			vertices[i].Normal = ((CHeightMapImage*)pContext)->GetHeightMapNormal(x, z);
			vertices[i].TexC = XMFLOAT2(float(x) / float(cxHeightMap - 1), float(czHeightMap - 1 - z) / float(czHeightMap - 1));
			vertices[i].TangentU = XMFLOAT3(float(x) / float(m_xmf3Scale.x * 0.5f), float(z) / float(m_xmf3Scale.z * 0.5f), 0);
			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
		}
	}
#endif

//#ifndef _WITH_TERRAIN_TESSELATION
	indices.resize(((nWidth * 2) * (nLength - 1)) + ((nLength - 1) - 1));
	for (int j = 0, z = 0; z < nLength - 1; z++)
	{
		if ((z % 2) == 0)
		{
			for (int x = 0; x < nWidth; x++)
			{
				if ((x == 0) && (z > 0))
					indices[j++] = (UINT)(x + (z * nWidth));
				indices[j++] = (UINT)(x + (z * nWidth));
				indices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
		else
		{
			for (int x = nWidth - 1; x >= 0; x--)
			{
				if (x == (nWidth - 1))
					indices[j++] = (UINT)(x + (z * nWidth));
				indices[j++] = (UINT)(x + (z * nWidth));
				indices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
	}
//#endif

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(FrameResource::Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(UINT);

	D3DCreateBlob(vbByteSize, &VertexBufferCPU);
	CopyMemory(VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);
	D3DCreateBlob(ibByteSize, &IndexBufferCPU);
	CopyMemory(IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	auto device = Graphics::Instance()->device;
	auto commandList = Graphics::Instance()->commandList;

	VertexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(), commandList.Get(), vertices.data(), vbByteSize, VertexBufferUploader);
	IndexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(), commandList.Get(), indices.data(), ibByteSize, IndexBufferUploader);

	VertexByteStride = sizeof(FrameResource::Vertex);
	VertexBufferByteSize = vbByteSize;
	IndexFormat = DXGI_FORMAT_R32_UINT;
	IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	DrawArgs["submesh"] = submesh;
}

CHeightMapGridMesh::~CHeightMapGridMesh()
{
}

float CHeightMapGridMesh::OnGetHeight(int x, int z, void *pContext)
{
	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	BYTE* pHeightMapPixels = pHeightMapImage->GetHeightMapPixels();
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
	int nWidth = pHeightMapImage->GetHeightMapWidth();
	float fHeight = pHeightMapPixels[x + (z * nWidth)] * xmf3Scale.y;
	return(fHeight);
}

XMFLOAT4 CHeightMapGridMesh::OnGetColor(int x, int z, void* pContext)
{
	//조명의 방향 벡터(정점에서 조명까지의 벡터)이다.
	XMFLOAT3 xmf3LightDirection = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	xmf3LightDirection = NS_Vector3::Normalize(xmf3LightDirection);
	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();

	//조명의 색상(세기, 밝기)이다.
	XMFLOAT4 xmf4IncidentLightColor(0.9f, 0.8f, 0.4f, 1.0f);

	/*정점 (x, z)에서 조명이 반사되는 양(비율)은 정점 (x, z)의 법선 벡터와 조명의 방향 벡터의 내적(cos)과 인접한 3개
	의 정점 (x+1, z), (x, z+1), (x+1, z+1)의 법선 벡터와 조명의 방향 벡터의 내적을 평균하여 구한다. 정점 (x, z)의 색
	상은 조명 색상(세기)과 반사되는 양(비율)을 곱한 값이다.*/
	float fScale = NS_Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z), xmf3LightDirection);
	fScale += NS_Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z), xmf3LightDirection);
	fScale += NS_Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z + 1), xmf3LightDirection);
	fScale += NS_Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z + 1), xmf3LightDirection);
	fScale = (fScale / 4.0f) + 0.05f;

	if (fScale > 1.0f)
		fScale = 1.0f;
	if (fScale < 0.25f)
		fScale = 0.25f;
	//fScale은 조명 색상(밝기)이 반사되는 비율이다.

	XMFLOAT4 xmf4Color = NS_Vector4::Multiply(fScale, xmf4IncidentLightColor);
	return(xmf4Color);
}
