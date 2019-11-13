#include "pch.h"
#include "Input.h"

Vector3 Input::mousePosition;
bool Input::mouseDown[3];

Input::Input()
{
}

Input::~Input()
{
}

void Input::Update()
{
	for (auto& d : mouseDown)
		d = false;
}

bool Input::GetMouseButtonDown(int button)
{
	return mouseDown[button];
}