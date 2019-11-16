#include "pch.h"
#include "Camera.h"

Camera* Camera::main{ nullptr };

Camera::Camera()
{
	m_xmf4x4View = NS_Matrix4x4::Identity();
	m_xmf4x4Projection = NS_Matrix4x4::Identity();
	m_d3dViewport = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	m_d3dScissorRect = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
}

Camera::~Camera()
{
}

void Camera::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4View;
	XMStoreFloat4x4(&xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4View, 0);

	XMFLOAT4X4 xmf4x4Projection;
	XMStoreFloat4x4(&xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4Projection, 16);
}

void Camera::GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up)
{
	m_xmf4x4View = NS_Matrix4x4::LookAtLH(xmf3Position, xmf3LookAt, xmf3Up);
}

void Camera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle)
{
	//m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
	float Size = 160;
	Size *= 2;
	XMStoreFloat4x4(&m_xmf4x4Projection, XMMatrixOrthographicLH(Size / 9.0 * 16, Size, 0.3, 1000));
	//1600 : 900 = x : 160
}

void Camera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX = float(xTopLeft);
	m_d3dViewport.TopLeftY = float(yTopLeft);
	m_d3dViewport.Width = float(nWidth);
	m_d3dViewport.Height = float(nHeight);
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;
}

void Camera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_d3dScissorRect.left = xLeft;
	m_d3dScissorRect.top = yTop;
	m_d3dScissorRect.right = xRight;
	m_d3dScissorRect.bottom = yBottom;
}

void Camera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}

Vector3 Camera::ScreenToWorldPoint(Vector3 position)
{
	position.x = position.x / m_d3dViewport.Width * 2 - 1;
	position.y = position.y / m_d3dViewport.Height * 2 - 1;
	position.y = -position.y;

	//position.x = position.x * 80 * 16 / 9;
	//position.y = position.y * 80;

	Vector3 vector;
	XMStoreFloat3(&vector.xmf3, XMVector3Transform(XMLoadFloat3(&position.xmf3), XMMatrixInverse(NULL, XMLoadFloat4x4(&m_xmf4x4Projection))));
	XMStoreFloat3(&vector.xmf3, XMVector3Transform(XMLoadFloat3(&vector.xmf3), XMMatrixInverse(NULL, XMLoadFloat4x4(&m_xmf4x4View))));

	return vector;
}
