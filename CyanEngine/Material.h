#pragma once

class Material : public Object
{
public:
	XMFLOAT4 albedo{ 1, 1, 1, 1 };

public:
	Shader* shader;

public:
	Material() = default;
	Material(Material&) = default;
};