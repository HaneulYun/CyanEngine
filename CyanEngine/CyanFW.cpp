#include "pch.h"
#include "scripts.h"
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

	scene = new GameScene();
	if (scene)
		scene->Start();

	Time::Instance()->Reset();
	Input::Instance();
	Random::Instance();
	Random::Start();

	return true;
}

void CyanFW::OnDestroy()
{
}

void CyanFW::FrameAdvance()
{
	Time::Instance()->Tick();


	scene->Update();
	scene->Render();

	Input::Update();

	Time::Instance()->GetFrameRate(m_pszFrameRate + 12, 37);
	SetWindowText(CyanWindow::m_hWnd, m_pszFrameRate);
}

void CyanFW::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		Input::mouseDown[0] = true;
		break;
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
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
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
			SceneManager::scenemanager->GetComponent<SceneManager>()->bulletType = wParam - '1';
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
