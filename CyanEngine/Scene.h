#pragma once

class Scene
{
private:
	std::deque<GameObject*> gameObjects;
	Renderer* renderer;

public:
	Scene();
	~Scene();

	void OnStart();

	void Update();
	void Render();

	void OnDestroy();

	//----------------//
	void BuildObjects(ID3D12Device* pd3dDevice = nullptr);
	void ReleaseObjects();
	void AnimateObjects(float fTimeElapsed = 0);

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	//----------------//
	void ReleaseUploadBuffers();

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();

private:
	GameObject** m_ppObjects = NULL;
	int m_nObjects = 0;

	ID3D12RootSignature* m_pd3dGraphicsRootSignature{ nullptr };
};

