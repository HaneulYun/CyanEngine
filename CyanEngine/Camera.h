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
	Matrix4x4 view;
	Matrix4x4 projection;

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
	
	void GenerateOrthoMatrix(float _width, float _height, float _near, float _far);

	Vector3 ScreenToWorldPoint(Vector3 position);
	Vector3 ScreenToViewportPoint(Vector3 position);
};