#pragma once

#define EPSILON 1.0e-10f

inline bool IsZero(float fValue) { return((fabsf(fValue) < EPSILON)); }
inline bool IsEqual(float fA, float fB) { return(::IsZero(fA - fB)); }
inline float InverseSqrt(float fValue) { return 1.0f / sqrtf(fValue); }
inline void Swap(float* pfS, float* pfT) { float fTemp = *pfS; *pfS = *pfT; *pfT = fTemp; }

struct Vector3
{
	union
	{
		XMFLOAT3 xmf3;
		struct
		{
			float x;
			float y;
			float z;
		};
		struct
		{
			float r;
			float g;
			float b;
		};
	};

	Vector3() = default;
	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

	bool operator==(const Vector3& rhs) const
	{
		return ((x == rhs.x) && (y == rhs.y) && (z == rhs.z));
	}
	bool operator!=(const Vector3& rhs) const
	{
		return !((x == rhs.x) && (y == rhs.y) && (z == rhs.z));
	}

	float Length() const
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3)));
		return(xmf3Result.x);
	}

	Vector3& Normalize()
	{
		XMStoreFloat3(&xmf3, XMVector3Normalize(XMLoadFloat3(&xmf3)));
		return *this;
	}
	Vector3 Normalized() const
	{
		Vector3 result;
		XMStoreFloat3(&result.xmf3, XMVector3Normalize(XMLoadFloat3(&xmf3)));
		return result;
	}
	Vector3 operator-(const Vector3& rhs) const
	{
		Vector3 result;
		XMStoreFloat3(&result.xmf3, XMLoadFloat3(&xmf3) - XMLoadFloat3(&rhs.xmf3));
		return result;
	}
	Vector3 operator+(const Vector3& rhs) const
	{
		Vector3 result;
		XMStoreFloat3(&result.xmf3, XMLoadFloat3(&xmf3) + XMLoadFloat3(&rhs.xmf3));
		return result;
	}
	Vector3 operator*(const Vector3& rhs) const
	{
		Vector3 result;
		XMStoreFloat3(&result.xmf3, XMLoadFloat3(&xmf3) * XMLoadFloat3(&rhs.xmf3));
		return result;
	}
	Vector3 operator/(const Vector3& rhs) const
	{
		Vector3 result;
		XMStoreFloat3(&result.xmf3, XMLoadFloat3(&xmf3) / XMLoadFloat3(&rhs.xmf3));
		return result;
	}
	Vector3 operator*(const float rhs) const
	{
		Vector3 result;
		XMStoreFloat3(&result.xmf3, XMLoadFloat3(&xmf3) * rhs);
		return result;
	}
	Vector3 operator/(const float rhs) const
	{
		Vector3 result;
		XMStoreFloat3(&result.xmf3, XMLoadFloat3(&xmf3) / rhs);
		return result;
	}

	Vector3& operator+=(const Vector3& rhs)
	{
		XMStoreFloat3(&xmf3, XMLoadFloat3(&xmf3) + XMLoadFloat3(&rhs.xmf3));
		return *this;
	}
	Vector3& operator-=(const Vector3& rhs)
	{
		XMStoreFloat3(&xmf3, XMLoadFloat3(&xmf3) - XMLoadFloat3(&rhs.xmf3));
		return *this;
	}
	Vector3& operator*=(const Vector3& rhs)
	{
		XMStoreFloat3(&xmf3, XMLoadFloat3(&xmf3) * XMLoadFloat3(&rhs.xmf3));
		return *this;
	}
	Vector3& operator/=(const Vector3& rhs)
	{
		XMStoreFloat3(&xmf3, XMLoadFloat3(&xmf3) / XMLoadFloat3(&rhs.xmf3));
		return *this;
	}
};

namespace NS_Vector3
{
	inline bool IsZero(XMFLOAT3& xmf3Vector)
	{
		if (::IsZero(xmf3Vector.x) && ::IsZero(xmf3Vector.y) && ::IsZero(xmf3Vector.z))
			return(true);
		return(false);
	}
	inline XMFLOAT3 XMVectorToFloat3(XMVECTOR& xmvVector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, xmvVector);
		return(xmf3Result);
	}
	inline XMFLOAT3 ScalarProduct(XMFLOAT3& xmf3Vector, float fScalar, bool bNormalize = true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)) * fScalar);
		else
			XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector) * fScalar);
		return(xmf3Result);
	}
	inline XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}
	inline XMFLOAT3 Add(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, float fScalar)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + (XMLoadFloat3(&xmf3Vector2) * fScalar));
		return(xmf3Result);
	}
	inline XMFLOAT3 Subtract(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}
	inline float DotProduct(const XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result.x);
	}
	inline XMFLOAT3 CrossProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, bool bNormalize = true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2))));
		else
			XMStoreFloat3(&xmf3Result, XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result);
	}
	inline XMFLOAT3 Normalize(const XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 m_xmf3Normal;
		XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)));
		return(m_xmf3Normal);
	}
	inline float Length(const XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3Vector)));
		return(xmf3Result.x);
	}
	inline float Angle(const XMVECTOR& xmvVector1, const XMVECTOR& xmvVector2)
	{
		XMVECTOR xmvAngle = XMVector3AngleBetweenNormals(xmvVector1, xmvVector2);
		return(XMConvertToDegrees(XMVectorGetX(xmvAngle)));
	}
	inline float Angle(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		return(Angle(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
	}
	inline XMFLOAT3 TransformNormal(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3TransformNormal(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
		return(xmf3Result);
	}
	inline XMFLOAT3 TransformCoord(const XMFLOAT3& xmf3Vector, const XMMATRIX& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3TransformCoord(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
		return(xmf3Result);
	}
	inline XMFLOAT3 TransformCoord(XMFLOAT3& xmf3Vector, XMFLOAT4X4& xmmtx4x4Matrix)
	{
		return(TransformCoord(xmf3Vector, XMLoadFloat4x4(&xmmtx4x4Matrix)));
	}
}