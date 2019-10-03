#include "pch.h"
#include "Shader.h"

Shader::~Shader()
{
	if (m_ppd3dPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; i++) if (m_ppd3dPipelineStates[i])
			m_ppd3dPipelineStates[i]->Release();
		delete[] m_ppd3dPipelineStates;
	}
}

D3D12_RASTERIZER_DESC Shader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	return(d3dRasterizerDesc);
}

D3D12_DEPTH_STENCIL_DESC Shader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return d3dDepthStencilDesc;
}

D3D12_BLEND_DESC Shader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return d3dBlendDesc;
}

D3D12_INPUT_LAYOUT_DESC Shader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE Shader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE Shader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE Shader::CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	::D3DCompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, NULL);
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();
	return(d3dShaderByteCode);
}

void Shader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
	ID3DBlob* pd3dVertexShaderBlob = NULL, * pd3dPixelShaderBlob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dRootSignature;
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)& m_ppd3dPipelineStates[0]);

	if (pd3dVertexShaderBlob)
		pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob)
		pd3dPixelShaderBlob->Release();
	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs)
		delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void Shader::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}
void Shader::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0]);
}

void Shader::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	OnPrepareRender(pd3dCommandList);
}

PlayerShader::PlayerShader()
{
}

PlayerShader::~PlayerShader()
{
}

D3D12_INPUT_LAYOUT_DESC PlayerShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE PlayerShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(Shader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE PlayerShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(Shader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1", ppd3dShaderBlob));
}

void PlayerShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	Shader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

ObjectsShader::ObjectsShader()
{
}

ObjectsShader::~ObjectsShader()
{
}

void ObjectsShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//가로x세로x높이가 12x12x12인 정육면체 메쉬를 생성한다.
	CubeMeshDiffused *pCubeMesh = new CubeMeshDiffused(pd3dDevice, pd3dCommandList,
	12.0f, 12.0f, 12.0f);
	/*x-축, y-축, z-축 양의 방향의 객체 개수이다. 각 값을 1씩 늘리거나 줄이면서 실행할 때 프레임 레이트가 어떻게
	변하는 가를 살펴보기 바란다.*/
	int xObjects = 10, yObjects = 10, zObjects = 10, i = 0;
	//x-축, y-축, z-축으로 21개씩 총 21 x 21 x 21 = 9261개의 정육면체를 생성하고 배치한다.
	m_nObjects = (xObjects * 2 + 1) * (yObjects * 2 + 1) * (zObjects * 2 + 1) - 1;
	m_ppObjects = new GameObject * [m_nObjects];
	float fxPitch = 12.0f * 2.5f;
	float fyPitch = 12.0f * 2.5f;
	float fzPitch = 12.0f * 2.5f;
	RotatingObject* pRotatingObject = NULL;
	for (int x = -xObjects; x <= xObjects; x++)
	{
		for (int y = -yObjects; y <= yObjects; y++)
		{
			for (int z = -zObjects; z <= zObjects; z++)
			{
				if (!x && !y && !z)
					continue;
				pRotatingObject = new RotatingObject();
				pRotatingObject->SetMesh(pCubeMesh);
				//각 정육면체 객체의 위치를 설정한다.
				pRotatingObject->SetPosition(fxPitch*x, fyPitch*y, fzPitch*z);
				pRotatingObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
				pRotatingObject->SetRotationSpeed(10.0f * (i % 10) + 3.0f);
				m_ppObjects[i++] = pRotatingObject;
			}
		}
	}
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void ObjectsShader::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);
	}
}

void ObjectsShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			if (m_ppObjects[j]) delete m_ppObjects[j];
		}
		delete[] m_ppObjects;
	}
}

D3D12_INPUT_LAYOUT_DESC ObjectsShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	
	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE ObjectsShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(Shader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE ObjectsShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(Shader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1", ppd3dShaderBlob));
}

void ObjectsShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];
	Shader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

void ObjectsShader::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) m_ppObjects[j]->ReleaseUploadBuffers();
	}
}

void ObjectsShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	Shader::Render(pd3dCommandList, pCamera);
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			m_ppObjects[j]->Render(pd3dCommandList);
		}
	}
}
