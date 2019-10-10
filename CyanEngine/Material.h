#pragma once

class Material : public Object
{
private:
	XMFLOAT4 albedo{ 0, 0, 0, 1 };

	D3D12_SHADER_BYTECODE ps{ NULL };
	D3D12_SHADER_BYTECODE vs{ NULL };
	D3D12_SHADER_BYTECODE ds{ NULL };
	D3D12_SHADER_BYTECODE hs{ NULL };
	D3D12_SHADER_BYTECODE gs{ NULL };

	D3D12_BLEND_DESC blendDesc;
	D3D12_RASTERIZER_DESC rasterizerDesc;
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;

public:
	ID3D12RootSignature* rootSignature;
	ID3D12PipelineState* pipelineState;

public:
	Shader* shader;
};