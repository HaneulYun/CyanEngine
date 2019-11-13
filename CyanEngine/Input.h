#pragma once

class Input : public Singleton<Input>
{
public:
	static Vector3 mousePosition;

public:
	Input();
	~Input();

	static void Update();
};