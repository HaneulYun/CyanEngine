#pragma once

class Vertex
{
protected:
	XMFLOAT3 position;

public:
	Vertex() = default;
	Vertex(Vertex&) = default;
	~Vertex() {}
};

class DiffusedVertex : public Vertex
{
protected:
	XMFLOAT4 color;

public:
	DiffusedVertex() = default;
	DiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse)
	{
		position = XMFLOAT3(x, y, z);
		color = xmf4Diffuse;
	}
	DiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse)
	{
		position = xmf3Position;
		color = xmf4Diffuse;
	}
	~DiffusedVertex() { }
};

class IlluminatedVertex : public Vertex
{
protected:
	XMFLOAT3 normal;

public:
	IlluminatedVertex()
	{
		position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
	IlluminatedVertex(float x, float y, float z, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f))
	{
		position = XMFLOAT3(x, y, z);
		normal = xmf3Normal;
	}
	IlluminatedVertex(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f))
	{
		position = xmf3Position;
		normal = xmf3Normal;
	}
	~IlluminatedVertex() { }
};

class CTexturedVertex : public Vertex
{
public:
	XMFLOAT2						m_xmf2TexCoord;

public:
	CTexturedVertex()
	{
		position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f);
	}
	CTexturedVertex(float x, float y, float z, XMFLOAT2 xmf2TexCoord)
	{
		position = XMFLOAT3(x, y, z);
		m_xmf2TexCoord = xmf2TexCoord;
	}
	CTexturedVertex(XMFLOAT3 xmf3Position, XMFLOAT2 xmf2TexCoord = XMFLOAT2(0.0f, 0.0f))
	{
		position = xmf3Position;
		m_xmf2TexCoord = xmf2TexCoord;
	}
	~CTexturedVertex() { }
};

class CDiffusedTexturedVertex : public DiffusedVertex
{
public:
	XMFLOAT2						m_xmf2TexCoord;

public:
	CDiffusedTexturedVertex()
	{
		position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f);
	}
	CDiffusedTexturedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse, XMFLOAT2 xmf2TexCoord)
	{
		position = XMFLOAT3(x, y, z);
		color = xmf4Diffuse;
		m_xmf2TexCoord = xmf2TexCoord;
	}
	CDiffusedTexturedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), XMFLOAT2 xmf2TexCoord = XMFLOAT2(0.0f, 0.0f))
	{
		position = xmf3Position;
		color = xmf4Diffuse;
		m_xmf2TexCoord = xmf2TexCoord;
	}
	~CDiffusedTexturedVertex() { }
};