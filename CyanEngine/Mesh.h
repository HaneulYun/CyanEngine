#pragma once
#include "Vertex.h"

class Mesh
{
public:
	Mesh() = default;
	virtual ~Mesh() = default;

protected:
	ComPtr<ID3D12Resource> m_pd3dVertexBuffer{ nullptr };
	ComPtr<ID3D12Resource> m_pd3dVertexUploadBuffer{ nullptr };
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	UINT m_nSlot = 0;
	UINT m_nVertices = 0;
	UINT m_nStride = 0;
	UINT m_nOffset = 0;

	UINT m_nType = 0;

protected:
	ComPtr<ID3D12Resource> m_pd3dIndexBuffer{ nullptr };
	ComPtr<ID3D12Resource> m_pd3dIndexUploadBuffer{ nullptr };
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;

	UINT m_nIndices = 0;
	UINT m_nStartIndex = 0;
	int m_nBaseVertex = 0;

public:
	virtual void Render(UINT nInstances = 1, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView = {});
};

class TriangleMesh : public Mesh
{
public:
	TriangleMesh();
	virtual ~TriangleMesh() { }
};

class Quad : public Mesh
{
public:
	Quad();
	virtual ~Quad() { }
};

class CubeMeshDiffused : public Mesh
{
public:
	CubeMeshDiffused(float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CubeMeshDiffused() { }
};

class MeshIlluminated : public Mesh
{
public:
	MeshIlluminated() = default;
	virtual ~MeshIlluminated() { }

public:
	void CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, int nVertices);
	void CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices);
	void CalculateTriangleStripVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices);
	void CalculateVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, int nVertices, UINT* pnIndices, int nIndices);
};

class CubeMeshIlluminated : public MeshIlluminated
{
public:
	CubeMeshIlluminated(float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CubeMeshIlluminated() { }
};

class CHeightMapImage
{
private:
	//높이 맵 이미지 픽셀(8-비트)들의 이차원 배열이다. 각 픽셀은 0~255의 값을 갖는다.
	BYTE *m_pHeightMapPixels;
	//높이 맵 이미지의 가로와 세로 크기이다.
	int m_nWidth;
	int m_nLength;
	//높이 맵 이미지를 실제로 몇 배 확대하여 사용할 것인가를 나타내는 스케일 벡터이다.
	XMFLOAT3 m_xmf3Scale;
public:
	CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	~CHeightMapImage(void);
	
	//높이 맵 이미지에서 (x, z) 위치의 픽셀 값에 기반한 지형의 높이를 반환한다.
	float GetHeight(float x, float z);
	
	//높이 맵 이미지에서 (x, z) 위치의 법선 벡터를 반환한다.
	XMFLOAT3 GetHeightMapNormal(int x, int z);
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	BYTE* GetHeightMapPixels() { return(m_pHeightMapPixels); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }
};

class CHeightMapGridMesh : public Mesh
{
protected:
	//격자의 크기(가로: x-방향, 세로: z-방향)이다.
	int m_nWidth;
	int m_nLength;
	/*격자의 스케일(가로: x-방향, 세로: z-방향, 높이: y-방향) 벡터이다.
	실제 격자 메쉬의 각 정점의 x-좌표, y-좌표, z-좌표는 스케일 벡터의 x-좌표, y-좌표, z-좌표로 곱한 값을 갖는다.
	즉, 실제 격자의 x-축 방향의 간격은 1이 아니라 스케일 벡터의 x-좌표가 된다.
	이렇게 하면 작은 격자(적은 정점)를 사용하더라도 큰 크기의 격자(지형)를 생성할수 있다.*/
	XMFLOAT3 m_xmf3Scale;
public:
	CHeightMapGridMesh( int xStart, int zStart, int nWidth, int nLength,
		XMFLOAT3 xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f),
		XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
		void* pContext = NULL);
	virtual ~CHeightMapGridMesh();

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	int GetWidth() { return(m_nWidth); }
	int GetLength() { return(m_nLength); }
	//격자의 좌표가 (x, z)일 때 교점(정점)의 높이를 반환하는 함수이다.
	virtual float OnGetHeight(int x, int z, void *pContext);
	//격자의 좌표가 (x, z)일 때 교점(정점)의 색상을 반환하는 함수이다.
	virtual XMFLOAT4 OnGetColor(int x, int z, void *pContext);
};

#define VERTEXT_POSITION			0x01
#define VERTEXT_COLOR				0x02
#define VERTEXT_NORMAL				0x04

class CMeshLoadInfo
{
public:
	CMeshLoadInfo() { }
	~CMeshLoadInfo() {}

public:
	char		m_pstrMeshName[256] = { 0 };

	UINT		m_nType = 0x00;

	XMFLOAT3	m_xmf3AABBCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3	m_xmf3AABBExtents = XMFLOAT3(0.0f, 0.0f, 0.0f);

	int			m_nVertices = 0;
	XMFLOAT3*	m_pxmf3Positions = NULL;
	XMFLOAT4*	m_pxmf4Colors = NULL;
	XMFLOAT3*	m_pxmf3Normals = NULL;

	int			m_nIndices = 0;
	UINT*		m_pnIndices = NULL;

	int			m_nSubMeshes = 0;
	int*		m_pnSubSetIndices = NULL;
	UINT**		m_ppnSubSetIndices = NULL;
};

class CMeshFromFile : public Mesh
{
public:
	CMeshFromFile(CMeshLoadInfo* pMeshInfo);
	virtual ~CMeshFromFile();

public:
	virtual void ReleaseUploadBuffers();

protected:
	ID3D12Resource*				m_pd3dPositionBuffer = NULL;
	ID3D12Resource*				m_pd3dPositionUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW	m_d3dPositionBufferView;

	int							m_nSubMeshes = 0;
	int*						m_pnSubSetIndices = NULL;

	ID3D12Resource**			m_ppd3dSubSetIndexBuffers = NULL;
	ID3D12Resource**			m_ppd3dSubSetIndexUploadBuffers = NULL;
	D3D12_INDEX_BUFFER_VIEW*	m_pd3dSubSetIndexBufferViews = NULL;

public:
	virtual void Render(int nSubSet);
};

class CMeshIlluminatedFromFile : public CMeshFromFile
{
public:
	CMeshIlluminatedFromFile(CMeshLoadInfo* pMeshInfo);
	virtual ~CMeshIlluminatedFromFile();

	virtual void ReleaseUploadBuffers();

protected:
	ID3D12Resource* m_pd3dNormalBuffer = NULL;
	ID3D12Resource* m_pd3dNormalUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dNormalBufferView;

public:
	virtual void Render(int nSubSet, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView);
};
