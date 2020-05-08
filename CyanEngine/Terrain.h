#pragma once

//	//높이 맵의 가로와 세로 크기이다.
//	int m_nWidth;
//	int m_nLength;
//	//지형을 실제로 몇 배 확대할 것인가를 나타내는 스케일 벡터이다.
//	XMFLOAT3 m_xmf3Scale;
//
//public:
//	//지형의 높이 맵으로 사용할 이미지이다.
//	TerrainData* m_pHeightMapImage;
//	// 이 영역에 public 변수를 선언하세요.
//
//	void LoadTerrain(LPCTSTR pFileName, int nWidth, int nLength , int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color)
//	{
//		m_nWidth = nWidth;
//		m_nLength = nLength;
//
//		int cxQuadsPerBlock = nBlockWidth - 1;
//		int czQuadsPerBlock = nBlockLength - 1;
//
//		m_xmf3Scale = xmf3Scale;
//
//		m_pHeightMapImage = new TerrainData(pFileName, nWidth, nLength, xmf3Scale);
//
//		long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
//		long czBlocks = (m_nLength - 1) / czQuadsPerBlock;
//
//		//m_nMeshes = cxBlocks * czBlocks;
//		//m_ppMeshes = new Mesh * [m_nMeshes];
//		//for (int i = 0; i < m_nMeshes; i++)
//		//	m_ppMeshes[i] = NULL;
//
//		RenderTexture* pHeightMapGridMesh = NULL;
//		for (int z = 0, zStart = 0; z < czBlocks; z++)
//		{
//			for (int x = 0, xStart = 0; x < cxBlocks; x++)
//			{
//				xStart = x * (nBlockWidth - 1);
//				zStart = z * (nBlockLength - 1);
//				if (!mesh)
//					mesh = new RenderTexture(xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
//				else
//					(new GameObject(gameObject))->GetComponent<Terrain>()->mesh = new RenderTexture(xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
//			}
//		}
//
//		//지형을 렌더링하기 위한 셰이더를 생성한다.
//		//CTerrainShader* pShader = new CTerrainShader();
//		//pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
//		//SetShader(pShader);
//	}
//
//	//지형의 높이를 계산하는 함수이다(월드 좌표계). 높이 맵의 높이에 스케일의 y를 곱한 값이다.
//	float GetHeight(float x, float z)
//	{
//		return(m_pHeightMapImage->GetHeight(x /
//			m_xmf3Scale.x, z / m_xmf3Scale.z) * m_xmf3Scale.y);
//	}
//	//지형의 법선 벡터를 계산하는 함수이다(월드 좌표계). 높이 맵의 법선 벡터를 사용한다.
//	XMFLOAT3 GetNormal(float x, float z)
//	{
//		return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z)).xmf3);
//	}
//
//	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
//	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }
//	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
//	//지형의 크기(가로/세로)를 반환한다. 높이 맵의 크기에 스케일을 곱한 값이다.
//	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
//	float GetLength() { return(m_nLength * m_xmf3Scale.z); }
//};

class Terrain : public MonoBehavior<Terrain>
{
private:

public:
	Mesh* mesh{ nullptr };

	TerrainData terrainData;

protected:
	friend class GameObject;
	friend class MonoBehavior<Terrain>;
	Terrain() = default;
	Terrain(Terrain&) = default;

public:
	~Terrain() {}

	void Start();

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
	}

	void Set();
};