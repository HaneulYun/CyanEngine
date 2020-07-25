#include "pch.h"
#include "RenderTexture.h"

RenderTexture::RenderTexture(int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, void* pContext)
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

	TerrainData* pHeightMapImage = (TerrainData*)pContext;
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
			vertices[i].Pos = Vector3((x * m_xmf3Scale.x), fHeight, (z * m_xmf3Scale.z));
			vertices[i].Normal = ((TerrainData*)pContext)->GetHeightMapNormal(x, z);
			vertices[i].TexC = Vector2(float(x) / float(cxHeightMap - 1), float(czHeightMap - 1 - z) / float(czHeightMap - 1));
			vertices[i].TangentU = Vector3(float(x) / float(m_xmf3Scale.x * 0.5f), float(z) / float(m_xmf3Scale.z * 0.5f), 0);
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

RenderTexture::~RenderTexture()
{
}

float RenderTexture::OnGetHeight(int x, int z, void* pContext)
{
	TerrainData* pHeightMapImage = (TerrainData*)pContext;
	float* pHeightMapPixels = pHeightMapImage->GetHeightMapPixels();
	int nWidth = pHeightMapImage->GetHeightMapWidth();
	int nLength = pHeightMapImage->GetHeightMapLength();
	if (x < 0) x = 0;
	else if (x >= nWidth) x = nWidth;
	if (z < 0) z = 0;
	else if (z >= nLength) x = nLength;
	float fHeight = pHeightMapPixels[x + (z * nWidth)] / 255.0 * pHeightMapImage->size.y;
	return(fHeight);
}

XMFLOAT4 RenderTexture::OnGetColor(int x, int z, void* pContext)
{
	//조명의 방향 벡터(정점에서 조명까지의 벡터)이다.
	Vector3 xmf3LightDirection = Vector3(-1.0f, 1.0f, 1.0f);
	xmf3LightDirection.Normalize();

	TerrainData* pHeightMapImage = (TerrainData*)pContext;

	//조명의 색상(세기, 밝기)이다.
	XMFLOAT4 xmf4IncidentLightColor(0.9f, 0.8f, 0.4f, 1.0f);

	/*정점 (x, z)에서 조명이 반사되는 양(비율)은 정점 (x, z)의 법선 벡터와 조명의 방향 벡터의 내적(cos)과 인접한 3개
	의 정점 (x+1, z), (x, z+1), (x+1, z+1)의 법선 벡터와 조명의 방향 벡터의 내적을 평균하여 구한다. 정점 (x, z)의 색
	상은 조명 색상(세기)과 반사되는 양(비율)을 곱한 값이다.*/
	float fScale = Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z), xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z), xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z + 1), xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z + 1), xmf3LightDirection);
	fScale = (fScale / 4.0f) + 0.05f;

	if (fScale > 1.0f)
		fScale = 1.0f;
	if (fScale < 0.25f)
		fScale = 0.25f;
	//fScale은 조명 색상(밝기)이 반사되는 비율이다.

	XMFLOAT4 xmf4Color = NS_Vector4::Multiply(fScale, xmf4IncidentLightColor);
	return(xmf4Color);
}
