#include "pch.h"
#include "Material.h"

Material::Material(const wchar_t* fileName)
{
	if(!fileName)
		shader = new StandardShader();
	else
	{
		CTexture* ppTextures = new CTexture(1, RESOURCE_TEXTURE2D, 0);
		ppTextures->LoadTextureFromFile(fileName, 0);

		TextureShader* _shader = new TextureShader();
		shader = _shader;
		_shader->CreateCbvSrvDescriptorHeaps(0, TEXTURES);
		_shader->CreateShaderResourceViews(ppTextures, 3, false);
		_shader->ppMaterials[0] = new CMaterial();
		_shader->ppMaterials[0]->SetTexture(ppTextures);
	}
}
