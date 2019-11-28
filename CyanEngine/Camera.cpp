#include "pch.h"
#include "Camera.h"

Camera* Camera::main{ nullptr };

Camera::Camera()
{
	view = NS_Matrix4x4::Identity();
	projection = NS_Matrix4x4::Identity();
}

Camera::~Camera()
{
}

void Camera::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4View;
	
	XMFLOAT4X4 world{ gameObject->GetMatrix() };
	_pos = this->pos;
	XMFLOAT3 lookAt = this->lookAt;
	XMFLOAT3 up { 0, 1, 0 };

	XMStoreFloat3(&_pos, XMVector3Transform(XMLoadFloat3(&_pos), XMLoadFloat4x4(&world)));
	XMStoreFloat3(&lookAt, XMVector3Transform(XMLoadFloat3(&lookAt), XMLoadFloat4x4(&world)));
	//XMStoreFloat3(&up, XMVector3Transform(XMLoadFloat3(&up), XMLoadFloat4x4(&world)));

	XMStoreFloat4x4(&xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&NS_Matrix4x4::LookAtLH(_pos, lookAt, up))));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4View, 0);

	XMFLOAT4X4 xmf4x4Projection;
	XMStoreFloat4x4(&xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&projection)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4Projection, 16);
}

void Camera::GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up)
{
	pos = xmf3Position;
	lookAt = xmf3LookAt;
	up = xmf3Up;
	view = NS_Matrix4x4::LookAtLH(xmf3Position, xmf3LookAt, xmf3Up);
}

void Camera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle)
{
	projection = NS_Matrix4x4::PerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
}

void Camera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	viewport.TopLeftX = float(xTopLeft);
	viewport.TopLeftY = float(yTopLeft);
	viewport.Width = float(nWidth);
	viewport.Height = float(nHeight);
	viewport.MinDepth = fMinZ;
	viewport.MaxDepth = fMaxZ;
}

void Camera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	scissorRect.left = xLeft;
	scissorRect.top = yTop;
	scissorRect.right = xRight;
	scissorRect.bottom = yBottom;
}

void Camera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &viewport);
	pd3dCommandList->RSSetScissorRects(1, &scissorRect);
}

Vector3 Camera::ScreenToWorldPoint(Vector3 position)
{
	position.x = position.x / viewport.Width * 2 - 1;
	position.y = position.y / viewport.Height * 2 - 1;
	position.y = -position.y;

	Vector3 vector;
	XMStoreFloat3(&vector.xmf3, XMVector3Transform(XMLoadFloat3(&position.xmf3), XMMatrixInverse(NULL, XMLoadFloat4x4(&projection))));
	XMStoreFloat3(&vector.xmf3, XMVector3Transform(XMLoadFloat3(&vector.xmf3), XMMatrixInverse(NULL, XMLoadFloat4x4(&view))));

	return vector;
}
