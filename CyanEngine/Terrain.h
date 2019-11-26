#pragma once
#include "framework.h"

class Terrain : public MeshFilter
{
private:
	// 이 영역에 private 변수를 선언하세요.

	//지형의 높이 맵으로 사용할 이미지이다.
	CHeightMapImage* m_pHeightMapImage;
	//높이 맵의 가로와 세로 크기이다.
	int m_nWidth;
	int m_nLength;
	//지형을 실제로 몇 배 확대할 것인가를 나타내는 스케일 벡터이다.
	XMFLOAT3 m_xmf3Scale;

public:
	// 이 영역에 public 변수를 선언하세요.

private:
	friend class GameObject;
	Terrain() = default;
	Terrain(Terrain&) = default;

public:
	~Terrain() {}
	virtual Component* Duplicate() { return new Terrain; };
	virtual Component* Duplicate(Component* component) { return new Terrain(*(Terrain*)component); }

	void Start()
	{
		// 초기화 코드를 작성하세요.
	}

	void Update()
	{
		// 업데이트 코드를 작성하세요.
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
	void LoadTerrain(LPCTSTR pFileName, int nWidth, int nLength , int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color)
	{
		//지형에 사용할 높이 맵의 가로, 세로의 크기이다.
		m_nWidth = nWidth;
		m_nLength = nLength;

		/*지형 객체는 격자 메쉬들의 배열로 만들 것이다.
		nBlockWidth, nBlockLength는 격자 메쉬 하나의 가로, 세로 크기이다.
		cxQuadsPerBlock, czQuadsPerBlock은 격자 메쉬의 가로 방향과 세로 방향 사각형의 개수이다.*/
		int cxQuadsPerBlock = nBlockWidth - 1;
		int czQuadsPerBlock = nBlockLength - 1;

		//xmf3Scale는 지형을 실제로 몇 배 확대할 것인가를 나타낸다.
		m_xmf3Scale = xmf3Scale;

		//지형에 사용할 높이 맵을 생성한다.
		m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

		//지형에서 가로 방향, 세로 방향으로 격자 메쉬가 몇 개가 있는 가를 나타낸다.
		long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
		long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

		//지형 전체를 표현하기 위한 격자 메쉬의 개수이다.
		//m_nMeshes = cxBlocks * czBlocks;

		//지형 전체를 표현하기 위한 격자 메쉬에 대한 포인터 배열을 생성한다.
		//m_ppMeshes = new Mesh * [m_nMeshes];
		//for (int i = 0; i < m_nMeshes; i++)
		//	m_ppMeshes[i] = NULL;

		CHeightMapGridMesh* pHeightMapGridMesh = NULL;
		for (int z = 0, zStart = 0; z < czBlocks; z++)
		{
			for (int x = 0, xStart = 0; x < cxBlocks; x++)
			{
				//지형의 일부분을 나타내는 격자 메쉬의 시작 위치(좌표)이다.
				xStart = x * (nBlockWidth - 1);
				zStart = z * (nBlockLength - 1);
				//지형의 일부분을 나타내는 격자 메쉬를 생성하여 지형 메쉬에 저장한다.
				mesh = new CHeightMapGridMesh(xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
				//SetMesh(x + (z * cxBlocks), pHeightMapGridMesh);
			}
		}

		//지형을 렌더링하기 위한 셰이더를 생성한다.
		//CTerrainShader* pShader = new CTerrainShader();
		//pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
		//SetShader(pShader);
	}

	//지형의 높이를 계산하는 함수이다(월드 좌표계). 높이 맵의 높이에 스케일의 y를 곱한 값이다.
	float GetHeight(float x, float z)
	{
		return(m_pHeightMapImage->GetHeight(x /
			m_xmf3Scale.x, z / m_xmf3Scale.z) * m_xmf3Scale.y);
	}
	//지형의 법선 벡터를 계산하는 함수이다(월드 좌표계). 높이 맵의 법선 벡터를 사용한다.
	XMFLOAT3 GetNormal(float x, float z)
	{
		return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z)));
	}

	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	//지형의 크기(가로/세로)를 반환한다. 높이 맵의 크기에 스케일을 곱한 값이다.
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }
};