#include "pch.h"
#include "CyanApp.h"

HWND CyanApp::hwnd{ nullptr };

int CyanApp::Run(CyanFW* cyanFW, HINSTANCE hInstance, int nCmdShow)
{
	int argc;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	//cyanFW->ParseCommandLineArgs(argv, argc);
	LocalFree(argv);
	
	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = L"CyanEngine";
	RegisterClassEx(&windowClass);
	
	RECT windowRect = { 0, 0, static_cast<LONG>(cyanFW->GetWidth()), static_cast<LONG>(cyanFW->GetHeight()) };
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
	AdjustWindowRect(&windowRect, dwStyle, FALSE);
	
	hwnd = CreateWindow(windowClass.lpszClassName, cyanFW->GetTitle(), dwStyle, CW_USEDEFAULT, CW_USEDEFAULT,
		windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, hInstance, cyanFW);
	
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)cyanFW);
	
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	
	cyanFW->OnCreate(hInstance, hwnd);
	
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			cyanFW->OnFrameAdvance();
			Time::Instance()->GetFrameRate(cyanFW->m_pszFrameRate + 12, 37);
			SetWindowText(hwnd, cyanFW->m_pszFrameRate);
		}
	}

	delete cyanFW;

	// Return this part of the WM_QUIT message to Windows.
	return static_cast<char>(msg.wParam);
}

LRESULT CALLBACK CyanApp::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CyanFW* cyanFW = reinterpret_cast<CyanFW*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	if (cyanFW)
		if (!cyanFW->OnProcessingWindowMessage(hWnd, message, wParam, lParam))
			return 0;

	switch (message)
	{
	case WM_SIZE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_MOUSEWHEEL:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
