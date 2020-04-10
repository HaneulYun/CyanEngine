#pragma once

class CyanFW : public Singleton<CyanFW>
{
private:
	UINT width;
	UINT height;
	float aspectRatio;

	std::wstring title;

public:
	Graphics* graphics{ nullptr };
	Scene* scene{ nullptr };

	_TCHAR m_pszFrameRate[50];

public:
	CyanFW(UINT width = 1280, UINT height = 720, std::wstring name = L"default");
	virtual ~CyanFW();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnSetScene(Scene* newScene);
	void OnFrameAdvance();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	UINT GetWidth() const { return width; }
	UINT GetHeight() const { return height; }
	float GetAspectRatio() const { return aspectRatio; }
	const WCHAR* GetTitle() const { return title.c_str(); }
};
