#pragma once

#define EPSILON 1.0e-10f

struct Matrix4x4;

inline bool IsZero(float fValue) { return((fabsf(fValue) < EPSILON)); }
inline bool IsEqual(float fA, float fB) { return(::IsZero(fA - fB)); }
inline float InverseSqrt(float fValue) { return 1.0f / sqrtf(fValue); }
inline void Swap(float* pfS, float* pfT) { float fTemp = *pfS; *pfS = *pfT; *pfT = fTemp; }

struct Vector2
{
	union
	{
		XMFLOAT2 xmf2;
		struct
		{
			float x;
			float y;
		};
	};

	Vector2() = default;
	Vector2(const Vector2&) = default;
	Vector2& operator=(const Vector2&) = default;

	Vector2(Vector2&&) = default;
	Vector2& operator=(Vector2&&) = default;

	Vector2(float x, float y) : x(x), y(y) {}
};

namespace NS_Vector4
{
	inline XMFLOAT4 Multiply(float fScalar, XMFLOAT4& xmf4Vector)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, fScalar * XMLoadFloat4(&xmf4Vector));
		return(xmf4Result);
	}
	inline XMFLOAT4 Add(const XMFLOAT4& xmf4Vector1, XMFLOAT4& xmf4Vector2)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) + XMLoadFloat4(&xmf4Vector2));
		return(xmf4Result);
	}
}

struct Vector4
{
	union
	{
		XMFLOAT4 xmf4;
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
		struct
		{
			float r;
			float g;
			float b;
			float a;
		};
	};

	Vector4() = default;
	Vector4(const Vector4&) = default;
	Vector4& operator=(const Vector4&) = default;

	Vector4(Vector4&&) = default;
	Vector4& operator=(Vector4&&) = default;

	Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

	static Vector4 QuaternionSlerp(Vector4 q0, Vector4 q1, float t)
	{
		Vector4 result;
		XMStoreFloat4(&result.xmf4, XMQuaternionSlerp(XMLoadFloat4(&q0.xmf4), XMLoadFloat4(&q1.xmf4), t));
		return result;
	}
};

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

	bool IsZero()
	{
		if (::IsZero(x) && ::IsZero(y) && ::IsZero(z))
			return true;
		return false;
	}

	Vector3 TransformCoord(const Matrix4x4& mtx);
	Vector3 TransformNormal(const Matrix4x4& mtx);

	static float DotProduct(const Vector3& v0, const Vector3& v1)
	{
		Vector3 result;
		XMStoreFloat3(&result.xmf3, XMVector3Dot(XMLoadFloat3(&v0.xmf3), XMLoadFloat3(&v1.xmf3)));
		return result.x;
	}
	static Vector3 CrossProduct(const Vector3& v0, const Vector3& v1)
	{
		Vector3 result;
		XMStoreFloat3(&result.xmf3, XMVector3Cross(XMLoadFloat3(&v0.xmf3), XMLoadFloat3(&v1.xmf3)));
		return result;
	}
	static float Angle(const Vector3& v0, const Vector3& v1)
	{
		Vector3 result;
		XMStoreFloat3(&result.xmf3, XMVector3AngleBetweenVectors(XMLoadFloat3(&v0.xmf3), XMLoadFloat3(&v1.xmf3)));
		return result.x;
	}
	static Vector3 Lerp(const Vector3& v0, const Vector3& v1, float t)
	{
		Vector3 result;
		XMStoreFloat3(&result.xmf3, XMVectorLerp(XMLoadFloat3(&v0.xmf3), XMLoadFloat3(&v1.xmf3), t));
		return result;
	}
	static Vector3 Normalize(const Vector3& v)
	{
		Vector3 result;
		XMStoreFloat3(&result.xmf3, XMVector3Normalize(XMLoadFloat3(&v.xmf3)));
		return result;
	}
};