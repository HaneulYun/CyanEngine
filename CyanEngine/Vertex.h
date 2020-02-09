#pragma once

class Vertex
{
public:
	XMFLOAT3 position{};

	Vertex() = default;
	Vertex(Vertex&) = default;
	~Vertex() {}
};

class DiffusedVertex : public Vertex
{
public:
	XMFLOAT4 color{};

	DiffusedVertex() = default;
	DiffusedVertex(XMFLOAT3 _position, XMFLOAT4 _color)
	{
		position = _position;
		color = _color;
	}
	~DiffusedVertex() { }
};

class IlluminatedVertex : public Vertex
{
protected:
	XMFLOAT3 normal{};

public:
	IlluminatedVertex() = default;
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
	XMFLOAT2 uv{};
	XMFLOAT3 normal{0, 1, 0};

public:
	CTexturedVertex() = default;
	CTexturedVertex(float x, float y, float z, XMFLOAT2 xmf2TexCoord)
	{
		position = XMFLOAT3(x, y, z);
		uv = xmf2TexCoord;
	}
	CTexturedVertex(XMFLOAT3 xmf3Position, XMFLOAT2 xmf2TexCoord = XMFLOAT2(0.0f, 0.0f))
	{
		position = xmf3Position;
		uv = xmf2TexCoord;
	}
	~CTexturedVertex() { }
};

class CDiffusedTexturedVertex : public DiffusedVertex
{
public:
	XMFLOAT2 uv{};

public:
	CDiffusedTexturedVertex() = default;
	CDiffusedTexturedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse, XMFLOAT2 xmf2TexCoord)
	{
		position = XMFLOAT3(x, y, z);
		color = xmf4Diffuse;
		uv = xmf2TexCoord;
	}
	CDiffusedTexturedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), XMFLOAT2 xmf2TexCoord = XMFLOAT2(0.0f, 0.0f))
	{
		position = xmf3Position;
		color = xmf4Diffuse;
		uv = xmf2TexCoord;
	}
	~CDiffusedTexturedVertex() { }
};

class CDiffused2TexturedVertex : public DiffusedVertex
{
public:
	XMFLOAT2 uv0{};
	XMFLOAT2 uv1{};

public:
	CDiffused2TexturedVertex() = default;
	CDiffused2TexturedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse, XMFLOAT2 xmf2TexCoord0, XMFLOAT2 xmf2TexCoord1)
	{
		position = XMFLOAT3(x, y, z);
		color = xmf4Diffuse;
		uv0 = xmf2TexCoord0;
		uv1 = xmf2TexCoord1;
	}
	CDiffused2TexturedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), XMFLOAT2 xmf2TexCoord0 = XMFLOAT2(0.0f, 0.0f), XMFLOAT2 xmf2TexCoord1 = XMFLOAT2(0.0f, 0.0f))
	{
		position = xmf3Position;
		color = xmf4Diffuse;
		uv0 = xmf2TexCoord0;
		uv1 = xmf2TexCoord1;
	}
	~CDiffused2TexturedVertex() { }
};

class SkinnedVertex
{
public:
	XMFLOAT3 position{};
	XMFLOAT3 normal{};
	XMFLOAT3 boneWeights{};
	XMINT4 boneIndices{};
};