#pragma once

struct Matrix4x4
{
	union
	{
		XMFLOAT4X4 xmf4x4 = MathHelper::Identity4x4();
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float m[4][4];
	};

	Matrix4x4() = default;
	Matrix4x4(const Matrix4x4&) = default;
	Matrix4x4& operator=(const Matrix4x4&) = default;

	Matrix4x4(Matrix4x4&&) = default;
	Matrix4x4& operator=(Matrix4x4&&) = default;

	bool operator==(const Matrix4x4& rhs) const
	{
		return memcmp(&xmf4x4, &rhs.xmf4x4, sizeof(float) * 16);
	}
	bool operator!=(const Matrix4x4& rhs) const
	{
		return !(*this == rhs);
	}
	Matrix4x4 operator*(const Matrix4x4 rhs) const
	{
		Matrix4x4 result;
		XMStoreFloat4x4(&result.xmf4x4, XMLoadFloat4x4(&this->xmf4x4) * XMLoadFloat4x4(&rhs.xmf4x4));
		return result;
	}

	Matrix4x4 Identity()
	{
		XMStoreFloat4x4(&xmf4x4, XMMatrixIdentity());
		return *this;
	}
	Matrix4x4 Inverse()
	{
		Matrix4x4 result;
		XMStoreFloat4x4(&result.xmf4x4, XMMatrixInverse(NULL, XMLoadFloat4x4(&this->xmf4x4)));
		return result;
	}
	Matrix4x4 Transpose()
	{
		Matrix4x4 result;
		XMStoreFloat4x4(&result.xmf4x4, XMMatrixTranspose(XMLoadFloat4x4(&this->xmf4x4)));
		return result;
	}
	Matrix4x4 PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
	{
		XMStoreFloat4x4(&this->xmf4x4, XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ));
		return *this;
	}
	Matrix4x4 LookAtLH(Vector3& eye, Vector3& lookAt, Vector3& up)
	{
		XMStoreFloat4x4(&this->xmf4x4, XMMatrixLookAtLH(XMLoadFloat3(&eye.xmf3), XMLoadFloat3(&lookAt.xmf3), XMLoadFloat3(&up.xmf3)));
		return *this;
	}

	static Matrix4x4 RotationAxis(Vector3 axis, float angle)
	{
		Matrix4x4 result;
		XMStoreFloat4x4(&result.xmf4x4, XMMatrixRotationAxis(XMLoadFloat3(&axis.xmf3), angle));
		return result;
	}
	static Matrix4x4 ScalingFromVector(Vector3 scale)
	{
		Matrix4x4 result;
		XMStoreFloat4x4(&result.xmf4x4, XMMatrixScalingFromVector(XMLoadFloat3(&scale.xmf3)));
		return result;
	}
};

namespace NS_Matrix4x4
{
	inline XMFLOAT4X4 Identity()
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixIdentity());
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 Multiply(const XMFLOAT4X4& xmmtx4x4Matrix1, const XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMMATRIX& xmmtxMatrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * xmmtxMatrix2);
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 Multiply(XMMATRIX& xmmtxMatrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, xmmtxMatrix1 * XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 Inverse(XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixInverse(NULL, XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 Transpose(XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixTranspose(XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ));
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 LookAtLH(XMFLOAT3& xmf3EyePosition, XMFLOAT3& xmf3LookAtPosition, XMFLOAT3& xmf3UpDirection)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookAtLH(XMLoadFloat3(&xmf3EyePosition), XMLoadFloat3(&xmf3LookAtPosition), XMLoadFloat3(&xmf3UpDirection)));
		return(xmmtx4x4Result);
	}
}