#pragma once
#include "Vertex.h"

class TriangleMesh : public Mesh
{
public:
	TriangleMesh();
	virtual ~TriangleMesh() { }
};

class Quad : public Mesh
{
public:
	Quad(float width = 1.f, float depth = 1.f);
	virtual ~Quad() { }
};

class CubeMeshDiffused : public Mesh
{
public:
	CubeMeshDiffused(float fWidth = 1.0f, float fHeight = 1.0f, float fDepth = 1.0f);
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
	//���� �� �̹��� �ȼ�(8-��Ʈ)���� ������ �迭�̴�. �� �ȼ��� 0~255�� ���� ���´�.
	BYTE *m_pHeightMapPixels;
	//���� �� �̹����� ���ο� ���� ũ���̴�.
	int m_nWidth;
	int m_nLength;
	//���� �� �̹����� ������ �� �� Ȯ���Ͽ� ����� ���ΰ��� ��Ÿ���� ������ �����̴�.
	XMFLOAT3 m_xmf3Scale;
public:
	CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	~CHeightMapImage(void);
	
	//���� �� �̹������� (x, z) ��ġ�� �ȼ� ���� ����� ������ ���̸� ��ȯ�Ѵ�.
	float GetHeight(float x, float z);
	
	//���� �� �̹������� (x, z) ��ġ�� ���� ���͸� ��ȯ�Ѵ�.
	XMFLOAT3 GetHeightMapNormal(int x, int z);
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	BYTE* GetHeightMapPixels() { return(m_pHeightMapPixels); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }
};

class CHeightMapGridMesh : public Mesh
{
protected:
	//������ ũ��(����: x-����, ����: z-����)�̴�.
	int m_nWidth;
	int m_nLength;
	/*������ ������(����: x-����, ����: z-����, ����: y-����) �����̴�.
	���� ���� �޽��� �� ������ x-��ǥ, y-��ǥ, z-��ǥ�� ������ ������ x-��ǥ, y-��ǥ, z-��ǥ�� ���� ���� ���´�.
	��, ���� ������ x-�� ������ ������ 1�� �ƴ϶� ������ ������ x-��ǥ�� �ȴ�.
	�̷��� �ϸ� ���� ����(���� ����)�� ����ϴ��� ū ũ���� ����(����)�� �����Ҽ� �ִ�.*/
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
	//������ ��ǥ�� (x, z)�� �� ����(����)�� ���̸� ��ȯ�ϴ� �Լ��̴�.
	virtual float OnGetHeight(int x, int z, void *pContext);
	//������ ��ǥ�� (x, z)�� �� ����(����)�� ������ ��ȯ�ϴ� �Լ��̴�.
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
	virtual void Render(int nSubSet);
	virtual void Render(UINT nInstances, int nSubSet);
};

class CCubeMeshTextured : public Mesh
{
public:
	CCubeMeshTextured(float fWidth = 1.0f, float fHeight = 1.0f, float fDepth = 1.0f);
	virtual ~CCubeMeshTextured() {}
};

class CTexturedRectMesh : public Mesh
{
public:
	CTexturedRectMesh(float fWidth = 1.0f, float fHeight = 1.0f, float fDepth = 1.0f, float fxPosition = 0.0f, float fyPosition = 0.0f, float fzPosition = 0.0f);
	virtual ~CTexturedRectMesh();
};

class WaveMesh : public Mesh
{
public:
	WaveMesh(float fWidth = 1.0f, float fHeight = 1.0f, float fDepth = 1.0f, float fxPosition = 0.0f, float fyPosition = 0.0f, float fzPosition = 0.0f);
	virtual ~WaveMesh() {}
};

class CRawFormatImage
{
protected:
	BYTE* m_pRawImagePixels = NULL;

	int							m_nWidth;
	int							m_nLength;

public:
	CRawFormatImage(LPCTSTR pFileName, int nWidth, int nLength, bool bFlipY = false);
	~CRawFormatImage(void);

	BYTE GetRawImagePixel(int x, int z)
	{
		return(m_pRawImagePixels[x + (z * m_nWidth)]);
	}
	void SetRawImagePixel(int x, int z, BYTE nPixel) { m_pRawImagePixels[x + (z * m_nWidth)] = nPixel; }

	BYTE* GetRawImagePixels() { return(m_pRawImagePixels); }

	int GetRawImageWidth() { return(m_nWidth); }
	int GetRawImageLength() { return(m_nLength); }
};

class MeshFromFbx : public Mesh
{
public:
	MeshFromFbx(FbxMesh* fbxMesh);
	virtual ~MeshFromFbx() {}
};