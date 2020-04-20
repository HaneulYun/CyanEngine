#pragma once

#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

struct ViewportRect
{
	float x;
	float y;
	float w;
	float h;
};

class Camera : public Component
{
public:
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;

public:
	float Near{ 0.3 };
	float Far{ 1000 };
	float FOV{ 60 };

	ViewportRect viewport{ 0, 0, 1, 1 };

	static Camera* main;

public:
	Camera();
	virtual ~Camera() {}

	void Start();
	
	Vector3 ScreenToWorldPoint(Vector3 position);
};