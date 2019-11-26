#pragma once

enum class KeyCode
{
	Alpha0='0', Alpha1, Alpha2, Alpha3, Alpha4, Alpha5, Alpha6, Alpha7, Alpha8, Alpha9,
	A='A', B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z
};

class Input : public Singleton<Input>
{
public:
	static Vector3 mousePosition;
	static bool keys[256];
	static bool keyDown[256];
	static bool mouseDown[3];

public:
	Input();
	~Input();

	static void Update();
	static bool GetKey(KeyCode key);
	static bool GetKeyDown(KeyCode key);
	static bool GetMouseButtonDown(int button);
};