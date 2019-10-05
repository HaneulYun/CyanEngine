#pragma once

class GameObject : public Object
{
public:
	std::deque<Component*> components;
	Transform* transform;

public:
	GameObject();
	virtual ~GameObject();

	virtual void Start();
	virtual void Update();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera, UINT nInstances);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera, UINT nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView);

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	void SetPosition(float x, float y, float z);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);

protected:
	Mesh* m_pMesh = NULL;
	Shader* m_pShader = NULL;

public:
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

public:
	void ReleaseUploadBuffers();
	virtual void SetMesh(Mesh* pMesh);
	virtual void SetShader(Shader* pShader);
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
};

class RotatingObject : public GameObject
{
private:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;

public:
	RotatingObject();
	virtual ~RotatingObject();

	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }
	virtual void Animate(float fTimeElapsed);
};