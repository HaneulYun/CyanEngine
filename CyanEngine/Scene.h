#pragma once

class Scene
{
private:
	std::deque<GameObject*> gameObjects;
	Component* renderer;

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

private:
	ID3D12RootSignature* m_pd3dGraphicsRootSignature{ nullptr };
	ID3D12PipelineState* m_pd3dPipelineState{ nullptr };
};

