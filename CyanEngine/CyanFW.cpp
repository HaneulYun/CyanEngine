#include "pch.h"
#include "CyanFW.h"

CyanFW::CyanFW()
{
	_tcscpy_s(m_pszFrameRate, _T("CyanEngine ("));
}

CyanFW::~CyanFW()
{
}

bool CyanFW::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	CyanWindow::m_hInstance = hInstance;
	CyanWindow::m_hWnd = hMainWnd;

	Time::Instance()->Reset();
	Input::Instance();
	Random::Instance();
	Random::Start();

	return true;
}

void CyanFW::OnSetScene(Scene* newScene)
{
	scene = newScene;
	if (scene)
		scene->Start();
}

int CyanFW::Run()
{
	MSG msg{};

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Time::Instance()->Tick();

			scene->Update();
			scene->Render();

			Input::Update();

			Time::Instance()->GetFrameRate(m_pszFrameRate + 12, 37);
			SetWindowText(CyanWindow::m_hWnd, m_pszFrameRate);
		}
	}

	return (int)msg.wParam;
}

void CyanFW::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		Input::mouses[0] = true;
		Input::mouseDown[0] = true;
		break;
	case WM_MBUTTONDOWN:
		Input::mouses[1] = true;
		Input::mouseDown[1] = true;
		break;
	case WM_RBUTTONDOWN:
		Input::mouses[2] = true;
		Input::mouseDown[2] = true;
		break;
	case WM_LBUTTONUP:
		Input::mouses[0] = false;
		Input::mouseUp[0] = true;
		break;
	case WM_MBUTTONUP:
		Input::mouses[1] = false;
		Input::mouseUp[1] = true;
		break;
	case WM_RBUTTONUP:
		Input::mouses[2] = false;
		Input::mouseUp[2] = true;
		break;
	case WM_MOUSEMOVE:
		Input::mousePosition = Vector3(LOWORD(lParam), HIWORD(lParam), 0);
		break;
	default:
		break;
	}
}

void CyanFW::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case VK_RETURN:
		case VK_F8:
			break;
		case VK_F9:
			RendererManager::Instance()->ChangeSwapChainState();
			break;
		default:
			break;
		}
		Input::keys[('a' <= wParam && wParam <= 'z') ? wParam - ('a' - 'A') : wParam] = false;
		Input::keyUp[('a' <= wParam && wParam <= 'z') ? wParam - ('a' - 'A') : wParam] = true;
		break;
	case WM_KEYDOWN:
		Input::keys[('a' <= wParam && wParam <= 'z') ? wParam - ('a' - 'A') : wParam] = true;
		Input::keyDown[('a' <= wParam && wParam <= 'z') ? wParam - ('a' - 'A') : wParam] = true;
		break;
	default:
		break;
	}
}

LRESULT CyanFW::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	{
		CyanWindow::m_nWndClientWidth = LOWORD(lParam);
		CyanWindow::m_nWndClientHeight = HIWORD(lParam);
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}

	return 0;
}
