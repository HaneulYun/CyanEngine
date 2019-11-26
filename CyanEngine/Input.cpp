#include "pch.h"
#include "Input.h"

Vector3 Input::mousePosition;
bool Input::keys[256];
bool Input::keyDown[256];
bool Input::mouseDown[3];

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
}

bool Input::GetKey(KeyCode key)
{
	return keys[(int)key];
}

bool Input::GetKeyDown(KeyCode key)
{
	return keyDown[(int)key];
}

bool Input::GetMouseButtonDown(int button)
{
	return mouseDown[button];
}