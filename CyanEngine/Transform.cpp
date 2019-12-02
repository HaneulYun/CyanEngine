#include "pch.h"
#include "Transform.h"

Transform::Transform()
{
	XMStoreFloat4x4(&localToWorldMatrix, XMMatrixIdentity());
}

Transform::~Transform()
{
}

void Transform::Rotate(const XMFLOAT3& axis, float angle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&axis), XMConvertToRadians(angle));
	localToWorldMatrix = NS_Matrix4x4::Multiply(mtxRotate, localToWorldMatrix);
}

void Transform::Scale(const XMFLOAT3& scale)
{
	XMMATRIX mtxRotate = XMMatrixScalingFromVector(XMLoadFloat3(&scale));
	localToWorldMatrix = NS_Matrix4x4::Multiply(mtxRotate, localToWorldMatrix);
}