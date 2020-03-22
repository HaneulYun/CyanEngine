#pragma once

class DefaultMaterial : public Material
{
public:
	DefaultMaterial()
	{
		Name = "bricks0";
		MatCBIndex = 0;
		DiffuseSrvHeapIndex = -1;
		DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
		Roughness = 0.1f;
	}
	virtual ~DefaultMaterial() {};
};
