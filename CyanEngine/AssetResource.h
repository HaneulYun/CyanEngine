#pragma once

struct MaterialData
{
	Vector4 DiffuseAlbedo{ 1.0f, 1.0f, 1.0f, 1.0f };
	Vector3 FresnelR0{ 0.01f, 0.01f, 0.01f };
	float Roughness{ 0.5f };

	Matrix4x4 MatTransform{ Matrix4x4::MatrixIdentity() };

	UINT DiffuseMapIndex{};
	UINT NormalMapIndex{};
	UINT EmissiveMapIndex{};
	UINT MaskMapIndex{};
};

struct AssetResource
{
public:
	std::unique_ptr<UploadBuffer<MaterialData>> MaterialBuffer{ nullptr };
};
