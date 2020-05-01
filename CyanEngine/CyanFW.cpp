#include "pch.h"
#include "CyanFW.h"

CyanFW::CyanFW(UINT width, UINT height, std::wstring name)
	 : width(width), height(height), title(name)
{
	instance = this;
	_tcscpy_s(m_pszFrameRate, _T("CyanEngine ("));

	aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

CyanFW::~CyanFW()
{
}

bool CyanFW::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	Time::Instance()->Reset();
	Input::Instance();
	Random::Instance()->Start();

	if (!graphics)
		(graphics = Graphics::Instance())->Initialize();
	if (!sceneManager)
		sceneManager = SceneManager::Instance();

	return true;
}

void CyanFW::OnFrameAdvance()
{
	Time::Instance()->Tick();

	if (sceneManager->nextScene)
	{
		Scene::scene = sceneManager->scene = sceneManager->nextScene;
		sceneManager->nextScene = nullptr;

		Camera::main = Scene::scene->camera;
	}

	if (sceneManager->scene->isDirty)
	{
		sceneManager->scene->isDirty = false;
		sceneManager->scene->Start();
	}

	sceneManager->scene->Update();
	graphics->Render();

	Input::Update();
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
	case WM_MOUSEWHEEL:
		Input::mouseWheel = GET_WHEEL_DELTA_WPARAM(wParam);
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
		case VK_F9:
			Graphics::Instance()->ChangeSwapChainState();
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
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}

	return 0;
}
