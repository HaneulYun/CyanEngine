#pragma once

class CyanFW
{
private:
	UINT width;
	UINT height;
	float aspectRatio;

	std::wstring title;


public:
	Scene* scene{ nullptr };
	_TCHAR m_pszFrameRate[50];

public:
	CyanFW(UINT width, UINT height, std::wstring name);
	~CyanFW();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnSetScene(Scene* newScene);
	int Run();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	UINT GetWidth() const { return width; }
	UINT GetHeight() const { return height; }
	const WCHAR* GetTitle() const { return title.c_str(); }
};
