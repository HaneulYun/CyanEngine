#pragma once

struct LightConstants
{
	Matrix4x4	CameraProj;
	Matrix4x4	CameraView;
	Matrix4x4	CameraInvView;
	Matrix4x4	ViewProj[4];
	Matrix4x4	ViewProjS[4];

	Vector3		EyePosW{ 0.0f, 0.0f, 0.0f };
	float		cbPerObjectPad0{ 0.0f };
	Vector3		CameraEyePosW{ 0.0f, 0.0f, 0.0f };
	UINT		CountShadowMap{ 0 };

	Vector2		RenderTargetSize{ 0.0f, 0.0f };
	Vector2		InvRenderTargetSize{ 0.0f, 0.0f };
	float		NearZ{ 0.0f };
	float		FarZ{ 0.0f };
	float		TotalTime{ 0.0f };
	float		DeltaTime{ 0.0f };

	XMFLOAT4	AmbientLight{ 0.0f, 0.0f, 0.0f, 1.0f };
};

struct LightResource
{
	LightResource() = default;
	LightResource(const LightResource&) = delete;
	LightResource& operator=(const LightResource&) = delete;
	~LightResource() = default;

	std::unique_ptr<UploadBuffer<LightConstants>> LightCB{ nullptr };
};
