#pragma once

struct Matrix4x4
{
	union
	{
		XMFLOAT4X4 xmf4x4
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		struct
		{
			Vector3 right;		float _00;
			Vector3 up;			float _01;
			Vector3 forward;	float _02;
			Vector3 position;	float _03;
		};
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

	Matrix4x4(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33) :
		_11(m00), _12(m01), _13(m02), _14(m03),
		_21(m10), _22(m11), _23(m12), _24(m13),
		_31(m20), _32(m21), _33(m22), _34(m23),
		_41(m30), _42(m31), _43(m32), _44(m33)
	{}

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
	static Matrix4x4 MatrixIdentity()
	{
		static Matrix4x4 I(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
		return I;
	}
	Matrix4x4 Inverse() const
	{
		Matrix4x4 result;
		XMStoreFloat4x4(&result.xmf4x4, XMMatrixInverse(&XMMatrixDeterminant(XMLoadFloat4x4(&this->xmf4x4)), XMLoadFloat4x4(&this->xmf4x4)));
		return result;
	}
	Matrix4x4 Transpose() const
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
	Matrix4x4 LookAtLH(const Vector3& eye, const Vector3& lookAt, const Vector3& up)
	{
		XMStoreFloat4x4(&this->xmf4x4, XMMatrixLookAtLH(XMLoadFloat3(&eye.xmf3), XMLoadFloat3(&lookAt.xmf3), XMLoadFloat3(&up.xmf3)));
		return *this;
	}
	static Matrix4x4 MatrixLookAtLH(const Vector3& eye, const Vector3& lookAt, const Vector3& up)
	{
		Matrix4x4 result;
		result.LookAtLH(eye, lookAt, up);
		return result;
	}
	
	Vector4 QuaternionRotationMatrix()
	{
		Vector4 result;
		XMStoreFloat4(&result.xmf4, XMQuaternionRotationMatrix(XMLoadFloat4x4(&xmf4x4)));
		return result;
	}

	static Matrix4x4 MatrixOrthographicOffCenterLH(float viewLeft, float viewRight, float viewBottom, float viewTop, float nearZ, float farZ)
	{
		Matrix4x4 result;
		XMStoreFloat4x4(&result.xmf4x4, XMMatrixOrthographicOffCenterLH(viewLeft, viewRight, viewBottom, viewTop, nearZ, farZ));
		return result;
	}

	static Matrix4x4 RotationX(float angle)
	{
		Matrix4x4 result;
		XMStoreFloat4x4(&result.xmf4x4, XMMatrixRotationX(angle));
		return result;
	}
	static Matrix4x4 RotationY(float angle)
	{
		Matrix4x4 result;
		XMStoreFloat4x4(&result.xmf4x4, XMMatrixRotationY(angle));
		return result;
	}
	static Matrix4x4 RotationZ(float angle)
	{
		Matrix4x4 result;
		XMStoreFloat4x4(&result.xmf4x4, XMMatrixRotationZ(angle));
		return result;
	}
	static Matrix4x4 RotationAxis(Vector3 axis, float angle/*radian*/)
	{
		Matrix4x4 result;
		XMStoreFloat4x4(&result.xmf4x4, XMMatrixRotationAxis(XMLoadFloat3(&axis.xmf3), angle));
		return result;
	}
	static Matrix4x4 MatrixScaling(float scaleX, float scaleY, float scaleZ)
	{
		Matrix4x4 result;
		XMStoreFloat4x4(&result.xmf4x4, XMMatrixScaling(scaleX, scaleY, scaleZ));
		return result;
	}
	static Matrix4x4 ScalingFromVector(Vector3 scale)
	{
		Matrix4x4 result;
		XMStoreFloat4x4(&result.xmf4x4, XMMatrixScalingFromVector(XMLoadFloat3(&scale.xmf3)));
		return result;
	}
	static Matrix4x4 MatrixAffineTransformation(Vector3 scaling, Vector4 rotationOrigin, Vector4 rotationQuaternion, Vector3 translation)
	{
		Matrix4x4 result;
		XMStoreFloat4x4(&result.xmf4x4, XMMatrixAffineTransformation(XMLoadFloat3(&scaling.xmf3), XMLoadFloat4(&rotationOrigin.xmf4), XMLoadFloat4(&rotationQuaternion.xmf4), XMLoadFloat3(&translation.xmf3)));
		return result;
	}
};

//namespace NS_Matrix4x4
//{
//	inline XMFLOAT4X4 Identity()
//	{
//		XMFLOAT4X4 xmmtx4x4Result;
//		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixIdentity());
//		return(xmmtx4x4Result);
//	}
//	inline XMFLOAT4X4 Multiply(const XMFLOAT4X4& xmmtx4x4Matrix1, const XMFLOAT4X4& xmmtx4x4Matrix2)
//	{
//		XMFLOAT4X4 xmmtx4x4Result;
//		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * XMLoadFloat4x4(&xmmtx4x4Matrix2));
//		return(xmmtx4x4Result);
//	}
//	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMMATRIX& xmmtxMatrix2)
//	{
//		XMFLOAT4X4 xmmtx4x4Result;
//		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * xmmtxMatrix2);
//		return(xmmtx4x4Result);
//	}
//	inline XMFLOAT4X4 Multiply(XMMATRIX& xmmtxMatrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
//	{
//		XMFLOAT4X4 xmmtx4x4Result;
//		XMStoreFloat4x4(&xmmtx4x4Result, xmmtxMatrix1 * XMLoadFloat4x4(&xmmtx4x4Matrix2));
//		return(xmmtx4x4Result);
//	}
//	inline XMFLOAT4X4 Inverse(XMFLOAT4X4& xmmtx4x4Matrix)
//	{
//		XMFLOAT4X4 xmmtx4x4Result;
//		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixInverse(NULL, XMLoadFloat4x4(&xmmtx4x4Matrix)));
//		return(xmmtx4x4Result);
//	}
//	inline XMFLOAT4X4 Transpose(XMFLOAT4X4& xmmtx4x4Matrix)
//	{
//		XMFLOAT4X4 xmmtx4x4Result;
//		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixTranspose(XMLoadFloat4x4(&xmmtx4x4Matrix)));
//		return(xmmtx4x4Result);
//	}
//	inline XMFLOAT4X4 PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
//	{
//		XMFLOAT4X4 xmmtx4x4Result;
//		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ));
//		return(xmmtx4x4Result);
//	}
//	inline XMFLOAT4X4 LookAtLH(XMFLOAT3& xmf3EyePosition, XMFLOAT3& xmf3LookAtPosition, XMFLOAT3& xmf3UpDirection)
//	{
//		XMFLOAT4X4 xmmtx4x4Result;
//		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookAtLH(XMLoadFloat3(&xmf3EyePosition), XMLoadFloat3(&xmf3LookAtPosition), XMLoadFloat3(&xmf3UpDirection)));
//		return(xmmtx4x4Result);
//	}
//}