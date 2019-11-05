#pragma once

class Scene
{
private:
	std::deque<GameObject*> gameObjects;
	RendererManager* rendererManager;

public:
	Scene();
	~Scene();

	void Start();
	void Update();
	void Render();
	void Destroy();

	//----------------//
	void BuildObjects(ID3D12Device* pd3dDevice = nullptr);
	void ReleaseObjects();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();

private:
	ID3D12RootSignature* m_pd3dGraphicsRootSignature{ nullptr };
};

