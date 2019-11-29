#include "pch.h"
#include "Material.h"

Material::Material(const wchar_t* fileName, ShaderMode mode)
{
	switch (mode)
	{
	case ShaderMode::Standard:
		if (!fileName)
			shader = new StandardShader();
		else
		{
			CTexture* ppTextures = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
			ppTextures->LoadTextureFromFile(fileName, 0);

			TextureShader* _shader = new TextureShader();
			shader = _shader;
			_shader->CreateCbvSrvDescriptorHeaps(0, TEXTURES);
			_shader->CreateShaderResourceViews(ppTextures, 1, 3, false);
			_shader->ppMaterials[0] = new CMaterial();
			_shader->ppMaterials[0]->SetTexture(ppTextures);
		}
		break;
	case ShaderMode::Skybox:
		{
			CTexture* ppTextures = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
			ppTextures->LoadTextureFromFile(fileName, 0);

			CSkyBoxShader* _shader = new CSkyBoxShader();
			shader = _shader;
			_shader->CreateCbvSrvDescriptorHeaps(0, TEXTURES);
			_shader->CreateShaderResourceViews(ppTextures, 1, 3, false);
			_shader->ppMaterials[0] = new CMaterial();
			_shader->ppMaterials[0]->SetTexture(ppTextures);
		}
		break;
	case ShaderMode::Terrain:
		{
			CTexture* ppTextures = new CTexture(3, RESOURCE_TEXTURE2D, 0, 3);
			ppTextures->LoadTextureFromFile(L"Texture/Base_Texture.dds", 0);
			ppTextures->LoadTextureFromFile(L"Texture/Detail_Texture_7.dds", 1);
			ppTextures->LoadTextureFromFile(L"Texture/HeightMap(Alpha).dds", 2);

			CTerrainShader* _shader = new CTerrainShader();
			shader = _shader;
			_shader->CreateCbvSrvDescriptorHeaps(0, 3);
			_shader->CreateShaderResourceViews(ppTextures, 1, 3, true);
			_shader->ppMaterials[0] = new CMaterial();
			_shader->ppMaterials[0]->SetTexture(ppTextures);
		}
		break;
	case ShaderMode::Billboard:
		if (!fileName)
			shader = new StandardShader();
		else
		{
			CTexture* ppTextures = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
			ppTextures->LoadTextureFromFile(fileName, 0);

			CBillboardObjectsShader* _shader = new CBillboardObjectsShader();
			shader = _shader;
			_shader->CreateCbvSrvDescriptorHeaps(0, TEXTURES);
			_shader->CreateShaderResourceViews(ppTextures, 1, 3, false);
			_shader->ppMaterials[0] = new CMaterial();
			_shader->ppMaterials[0]->SetTexture(ppTextures);
		}
		break;
	default:
		break;
	}
}
