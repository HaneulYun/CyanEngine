#include "pch.h"
#include "Input.h"

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