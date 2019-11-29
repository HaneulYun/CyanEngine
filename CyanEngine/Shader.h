#pragma once

class GameObject;

class Camera;

class Shader
{
public:
	Shader() {}
	virtual ~Shader();

public:
	ID3D12RootSignature* rootSignature;
	//D3D12_SHADER_BYTECODE VS;
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	//D3D12_SHADER_BYTECODE PS;
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	//D3D12_SHADER_BYTECODE DS;
	//D3D12_SHADER_BYTECODE HS;
	//D3D12_SHADER_BYTECODE GS;
	//D3D12_STREAM_OUTPUT_DESC StreamOutput;
	//D3D12_BLEND_DESC BlendState;
	virtual D3D12_BLEND_DESC CreateBlendState();
	//UINT SampleMask;
	//D3D12_RASTERIZER_DESC RasterizerState;
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	//D3D12_DEPTH_STENCIL_DESC DepthStencilState;
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	//D3D12_INPUT_LAYOUT_DESC InputLayout;
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	//D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBStripCutValue;
	//D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType;
	//UINT NumRenderTargets;
	//DXGI_FORMAT RTVFormats[8];
	//DXGI_FORMAT DSVFormat;
	//DXGI_SAMPLE_DESC SampleDesc;
	//UINT NodeMask;
	//D3D12_CACHED_PIPELINE_STATE CachedPSO;
	//D3D12_PIPELINE_STATE_FLAGS Flags;

public:
	ComPtr<ID3D12PipelineState> pipelineState;

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);
	
	void CreateCbvSrvDescriptorHeaps(int nConstantBufferViews, int nShaderResourceViews);
	void CreateConstantBufferViews(int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride);
	void CreateShaderResourceViews(CTexture* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex, bool bAutoIncrement);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleForHeapStart() { return(m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart()); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleForHeapStart() { return(m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()); }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorStartHandle() { return(m_d3dCbvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorStartHandle() { return(m_d3dCbvGPUDescriptorStartHandle); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorStartHandle() { return(m_d3dSrvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorStartHandle() { return(m_d3dSrvGPUDescriptorStartHandle); }

public:
	ID3D12DescriptorHeap* m_pd3dCbvSrvDescriptorHeap = NULL;

	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorStartHandle;

public:
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* _device);
};

class StandardShader : public Shader
{
public:
	D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;
	D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob) override;
	D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob) override;
};

class TextureShader : public Shader
{
public:
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob) override;
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob) override;

	CMaterial* ppMaterials[TEXTURES];
};

class CTerrainShader : public TextureShader
{
public:
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
};

class CSkyBoxShader : public TextureShader
{
public:
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
};

class CBillboardObjectsShader : public TextureShader
{
public:
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);

	//CMaterial* m_pBillboardMaterial;
	//
	//ID3D12Resource* m_pd3dVertexBuffer = NULL;
	//ID3D12Resource* m_pd3dVertexUploadBuffer = NULL;
	//D3D12_VERTEX_BUFFER_VIEW		m_d3dVertexBufferView;
	//
	//int								m_nInstances = 0;
	//ID3D12Resource* m_pd3dInstancesBuffer = NULL;
	//ID3D12Resource* m_pd3dInstanceUploadBuffer = NULL;
	//D3D12_VERTEX_BUFFER_VIEW		m_d3dInstancingBufferView;
};