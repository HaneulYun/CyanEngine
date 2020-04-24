#include "pch.h"
#include "Vector3.h"

Vector3& Vector3::operator/=(const Vector3& rhs)
{
	XMStoreFloat3(&xmf3, XMLoadFloat3(&xmf3) / XMLoadFloat3(&rhs.xmf3));
	return *this;
}
Vector3 Vector3::TransformCoord(const Matrix4x4& mtx)
{
	Vector3 result;
	XMStoreFloat3(&result.xmf3, XMVector3TransformCoord(XMLoadFloat3(&this->xmf3), XMLoadFloat4x4(&mtx.xmf4x4)));
	return result;
}
Vector3 Vector3::TransformNormal(const Matrix4x4& mtx)
{
	Vector3 result;
	XMStoreFloat3(&result.xmf3, XMVector3TransformNormal(XMLoadFloat3(&this->xmf3), XMLoadFloat4x4(&mtx.xmf4x4)));
	return result;
}