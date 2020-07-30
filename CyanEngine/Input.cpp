#include "pch.h"
#include "Input.h"

wchar_t Input::buffer[8];
wchar_t Input::cbuffer[8];

Vector3 Input::mousePosition;
bool Input::keys[256];
bool Input::keyUp[256];
bool Input::keyDown[256];
bool Input::mouses[3];
bool Input::mouseUp[3];
bool Input::mouseDown[3];
float Input::mouseWheel;

Input::Input()
{
}

Input::~Input()
{
}

void Input::Update()
{
	for (auto& d : keyDown)
		d = false;
	for (auto& d : mouseDown)
		d = false;
	for (auto& d : mouseUp)
		d = false;
	mouseWheel = 0.0f;

	memset(buffer, 0, 8);
}

bool Input::GetKey(KeyCode key)
{
	return keys[(int)key];
}

bool Input::GetKeyUp(KeyCode key)
{
	return keyUp[(int)key];
}

bool Input::GetKeyDown(KeyCode key)
{
	return keyDown[(int)key];
}

bool Input::GetMouseButton(int button)
{
	return mouses[button];
}

bool Input::GetMouseButtonUp(int button)
{
	return mouseUp[button];
}

bool Input::GetMouseButtonDown(int button)
{
	return mouseDown[button];
}

float Input::GetMouseWheelDelta()
{
	return mouseWheel;
}

int Input::ProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	int len;
	HIMC himc = NULL;   // IME �ڵ�

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

	case WM_KEYUP:
		switch (wParam)
		{
		case VK_OEM_PERIOD:
			Input::keys[(int)KeyCode::Period] = false;
			Input::keyUp[(int)KeyCode::Period] = true;
			break;
		}
		Input::keys[('a' <= wParam && wParam <= 'z') ? wParam - ('a' - 'A') : wParam] = false;
		Input::keyUp[('a' <= wParam && wParam <= 'z') ? wParam - ('a' - 'A') : wParam] = true;
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_OEM_PERIOD:
			Input::keys[(int)KeyCode::Period] = true;
			Input::keyDown[(int)KeyCode::Period] = true;
			break;
		}
		Input::keys[('a' <= wParam && wParam <= 'z') ? wParam - ('a' - 'A') : wParam] = true;
		Input::keyDown[('a' <= wParam && wParam <= 'z') ? wParam - ('a' - 'A') : wParam] = true;
		break;

	case WM_IME_COMPOSITION:
		himc = ImmGetContext(hWnd);

		if (lParam & GCS_RESULTSTR)
		{
			if ((len = ImmGetCompositionString(himc, GCS_RESULTSTR, NULL, 0)) > 0)
			{
				ImmGetCompositionString(himc, GCS_RESULTSTR, cbuffer, len);
				cbuffer[len] = 0;

				wcscpy(buffer, cbuffer);
				//bufferLen = wcslen(cbuffer);
				memset(cbuffer, 0, 8);
			}
		}
		else if (lParam & GCS_COMPSTR)
		{
			len = ImmGetCompositionString(himc, GCS_COMPSTR, NULL, 0);
			ImmGetCompositionString(himc, GCS_COMPSTR, cbuffer, len);
			cbuffer[len] = 0;
		}

		ImmReleaseContext(hWnd, himc);
		return 0;
	case WM_IME_CHAR:
		return 0;
	case WM_CHAR:
		//if (wParam == VK_BACK)
		//{
		//	if (wcslen(buffer))
		//	{
		//		buffer[wcslen(buffer) - 1] = 0;
		//		--bufferLen;
		//	}
		//}
		//else if (wParam != 13)
		//{
		//buffer[wcslen(buffer)] = wParam;
		//++bufferLen;
		//}
		buffer[0] = wParam;
		return 0;
	}
	return 1;
}