#include "pch.h"
#include "Camera.h"

Camera* Camera::main{ nullptr };

Camera::Camera()
{
	view = Matrix4x4::MatrixIdentity();
	projection = Matrix4x4::MatrixIdentity();
}

void Camera::Start()
{
	projection.PerspectiveFovLH(XMConvertToRadians(FOV), CyanFW::Instance()->GetAspectRatio(), Near, Far);
}

void Camera::GenerateOrthoMatrix(float _width, float _height, float _near, float _far)
{
	XMStoreFloat4x4(&projection.xmf4x4, XMMatrixOrthographicLH(_width, _height, _near, _far));
}


Vector3 Camera::ScreenToWorldPoint(Vector3 position)
{
	position.x = (position.x / CyanFW::Instance()->GetWidth() * 2 - 1) / projection.xmf4x4(0, 0);
	position.y = (position.y / CyanFW::Instance()->GetHeight() * -2 + 1) / projection.xmf4x4(1, 1);

	auto transform = gameObject->GetComponent<Transform>();
	auto vLookAt = transform->position + transform->forward.Normalized();
	XMVECTOR pos = XMLoadFloat3(&transform->position.xmf3);
	XMVECTOR lookAt = XMLoadFloat3(&vLookAt.xmf3);
	XMVECTOR up{ 0, 1, 0 };

	Vector3 vector;
	//XMStoreFloat3(&vector.xmf3, XMVector3Transform(XMLoadFloat3(&position.xmf3), XMMatrixInverse(NULL, XMLoadFloat4x4(&projection.xmf4x4))));
	XMStoreFloat4x4(&view.xmf4x4, XMMatrixLookAtLH(pos, lookAt, up));
	XMStoreFloat3(&vector.xmf3, XMVector3TransformNormal(XMLoadFloat3(&position.xmf3), XMMatrixInverse(&XMMatrixDeterminant(XMLoadFloat4x4(&view.xmf4x4)), XMLoadFloat4x4(&view.xmf4x4))));
	return vector;
}

Vector3 Camera::ScreenToViewportPoint(Vector3 position)
{
	position.x = position.x / CyanFW::Instance()->GetWidth() * 2 - 1;
	position.y = position.y / CyanFW::Instance()->GetHeight() * 2 - 1;
	position.y = -position.y;

	return position;
}