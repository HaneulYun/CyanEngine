#include "pch.h"
#include "CyanFW.h"

CyanFW::CyanFW()
{
}

CyanFW::~CyanFW()
{
}

bool CyanFW::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	Renderer::Instance()->m_hInstance = hInstance;
	Renderer::Instance()->m_hWnd = hMainWnd;

	scene = new Scene();
	scene->OnStart();

	BuildObjects();

	return true;
}

void CyanFW::OnDestroy()
{
	ReleaseObjects();

	scene->OnDestroy();
}

void CyanFW::BuildObjects()
{
}

void CyanFW::ReleaseObjects()
{
}

void CyanFW::ProcessInput()
{
}

void CyanFW::AnimateObjects()
{
}

void CyanFW::FrameAdvance()
{
	scene->Update();
	scene->Render();

	ProcessInput();
	AnimateObjects();
}

void CyanFW::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		break;
	case WM_MOUSEMOVE:
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
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
		case VK_F8:
			break;
		case VK_F9:
			Renderer::Instance()->ChangeSwapChainState();
			break;
		default:
			break;
		}
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
		Renderer::Instance()->m_nWndClientWidth = LOWORD(lParam);
		Renderer::Instance()->m_nWndClientHeight = HIWORD(lParam);
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
