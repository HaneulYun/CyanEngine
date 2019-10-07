#pragma once

class ShaderManager : public Singleton<ShaderManager>
{
private:

public:
	ShaderManager();
	~ShaderManager();

	static D3D12_SHADER_BYTECODE CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);
};

