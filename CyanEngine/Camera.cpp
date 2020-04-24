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
	position.x = (position.x / CyanFW::Instance()->GetWidth() * 2 - 1) / projection(0, 0);
	position.y = (position.y / CyanFW::Instance()->GetHeight() * -2 + 1) / projection(1, 1);

	auto transform = gameObject->GetComponent<Transform>();
	auto vLookAt = transform->position + transform->forward.Normalized();
	XMVECTOR pos = XMLoadFloat3(&transform->position.xmf3);
	XMVECTOR lookAt = XMLoadFloat3(&vLookAt.xmf3);
	XMVECTOR up{ 0, 1, 0 };

	XMStoreFloat4x4(&view, XMMatrixLookAtLH(pos, lookAt, up));

	Vector3 vector;

	XMStoreFloat3(&vector.xmf3, XMVector3TransformNormal(XMLoadFloat3(&position.xmf3), XMMatrixInverse(&XMMatrixDeterminant(XMLoadFloat4x4(&view)), XMLoadFloat4x4(&view))));

	return vector;
}