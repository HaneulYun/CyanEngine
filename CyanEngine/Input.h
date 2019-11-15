#pragma once

class Input : public Singleton<Input>
{
public:
	static Vector3 mousePosition;
	static bool mouseDown[3];

public:
	Input();
	~Input();

	static void Update();
	static bool GetMouseButtonDown(int button);
};