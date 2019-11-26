#pragma once
#include "framework.h"

class Terrain : public MeshFilter
{
private:
	// �� ������ private ������ �����ϼ���.

	//������ ���� ������ ����� �̹����̴�.
	CHeightMapImage* m_pHeightMapImage;
	//���� ���� ���ο� ���� ũ���̴�.
	int m_nWidth;
	int m_nLength;
	//������ ������ �� �� Ȯ���� ���ΰ��� ��Ÿ���� ������ �����̴�.
	XMFLOAT3 m_xmf3Scale;

public:
	// �� ������ public ������ �����ϼ���.

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
		// �ʱ�ȭ �ڵ带 �ۼ��ϼ���.
	}

	void Update()
	{
		// ������Ʈ �ڵ带 �ۼ��ϼ���.
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
	void LoadTerrain(LPCTSTR pFileName, int nWidth, int nLength , int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color)
	{
		//������ ����� ���� ���� ����, ������ ũ���̴�.
		m_nWidth = nWidth;
		m_nLength = nLength;

		/*���� ��ü�� ���� �޽����� �迭�� ���� ���̴�.
		nBlockWidth, nBlockLength�� ���� �޽� �ϳ��� ����, ���� ũ���̴�.
		cxQuadsPerBlock, czQuadsPerBlock�� ���� �޽��� ���� ����� ���� ���� �簢���� �����̴�.*/
		int cxQuadsPerBlock = nBlockWidth - 1;
		int czQuadsPerBlock = nBlockLength - 1;

		//xmf3Scale�� ������ ������ �� �� Ȯ���� ���ΰ��� ��Ÿ����.
		m_xmf3Scale = xmf3Scale;

		//������ ����� ���� ���� �����Ѵ�.
		m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

		//�������� ���� ����, ���� �������� ���� �޽��� �� ���� �ִ� ���� ��Ÿ����.
		long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
		long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

		//���� ��ü�� ǥ���ϱ� ���� ���� �޽��� �����̴�.
		//m_nMeshes = cxBlocks * czBlocks;

		//���� ��ü�� ǥ���ϱ� ���� ���� �޽��� ���� ������ �迭�� �����Ѵ�.
		//m_ppMeshes = new Mesh * [m_nMeshes];
		//for (int i = 0; i < m_nMeshes; i++)
		//	m_ppMeshes[i] = NULL;

		CHeightMapGridMesh* pHeightMapGridMesh = NULL;
		for (int z = 0, zStart = 0; z < czBlocks; z++)
		{
			for (int x = 0, xStart = 0; x < cxBlocks; x++)
			{
				//������ �Ϻκ��� ��Ÿ���� ���� �޽��� ���� ��ġ(��ǥ)�̴�.
				xStart = x * (nBlockWidth - 1);
				zStart = z * (nBlockLength - 1);
				//������ �Ϻκ��� ��Ÿ���� ���� �޽��� �����Ͽ� ���� �޽��� �����Ѵ�.
				mesh = new CHeightMapGridMesh(xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
				//SetMesh(x + (z * cxBlocks), pHeightMapGridMesh);
			}
		}

		//������ �������ϱ� ���� ���̴��� �����Ѵ�.
		//CTerrainShader* pShader = new CTerrainShader();
		//pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
		//SetShader(pShader);
	}

	//������ ���̸� ����ϴ� �Լ��̴�(���� ��ǥ��). ���� ���� ���̿� �������� y�� ���� ���̴�.
	float GetHeight(float x, float z)
	{
		return(m_pHeightMapImage->GetHeight(x /
			m_xmf3Scale.x, z / m_xmf3Scale.z) * m_xmf3Scale.y);
	}
	//������ ���� ���͸� ����ϴ� �Լ��̴�(���� ��ǥ��). ���� ���� ���� ���͸� ����Ѵ�.
	XMFLOAT3 GetNormal(float x, float z)
	{
		return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z)));
	}

	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	//������ ũ��(����/����)�� ��ȯ�Ѵ�. ���� ���� ũ�⿡ �������� ���� ���̴�.
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }
};