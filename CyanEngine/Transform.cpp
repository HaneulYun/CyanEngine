#include "pch.h"
#include "Transform.h"

Transform::Transform()
{
	XMStoreFloat4x4(&localToWorldMatrix, XMMatrixIdentity());
}

Transform::~Transform()
{
}
