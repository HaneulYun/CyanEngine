#pragma once

enum class ShaderMode
{
	Standard, Skybox, Terrain, Billboard, Skinned
};

class Material : public Object
{
public:
	XMFLOAT4 albedo{ 1, 1, 1, 1 };

public:
	Shader* shader{ nullptr };
	CTexture* mainTexture{ nullptr };

public:
	Material(Material&) = default;

	Material(const wchar_t* fileName = nullptr, ShaderMode mode = ShaderMode::Standard);
};