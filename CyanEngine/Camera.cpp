#include "pch.h"
#include "Camera.h"

Camera* Camera::main{ nullptr };

Camera::Camera()
{
	view = NS_Matrix4x4::Identity();
	projection = NS_Matrix4x4::Identity();
}

void Camera::Start()
{
	projection = NS_Matrix4x4::PerspectiveFovLH(XMConvertToRadians(FOV), CyanFW::Instance()->GetAspectRatio(), Near, Far);
}

Vector3 Camera::ScreenToWorldPoint(Vector3 position)
{
	position.x = position.x / CyanFW::Instance()->GetWidth() * 2 - 1;
	position.y = position.y / CyanFW::Instance()->GetHeight() * 2 - 1;
	position.y = -position.y;

	Vector3 vector;
	XMStoreFloat3(&vector.xmf3, XMVector3Transform(XMLoadFloat3(&position.xmf3), XMMatrixInverse(NULL, XMLoadFloat4x4(&projection))));
	XMStoreFloat3(&vector.xmf3, XMVector3Transform(XMLoadFloat3(&vector.xmf3), XMMatrixInverse(NULL, XMLoadFloat4x4(&view))));

	return vector;
}

Vector3 Camera::ScreenToViewportPoint(Vector3 position)
{
	position.x = position.x / CyanFW::Instance()->GetWidth() * 2 - 1;
	position.y = position.y / CyanFW::Instance()->GetHeight() * 2 - 1;
	position.y = -position.y;

	return position;
}