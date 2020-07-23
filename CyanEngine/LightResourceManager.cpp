#include "pch.h"
#include "LightResourceManager.h"

void LightResourceManager::Init()
{
	sceneBounds.Center = XMFLOAT3(540.f, 0.0f, 540.f);
	float width = 200, height = 200;
	sceneBounds.Radius = sqrtf(width * width + height * height);

	auto graphics = Graphics::Instance();
	shadowMap = std::make_unique<ShadowMap>(graphics->device.Get(), 2048, 2048);
	shadowMap->BuildDescriptors(graphics->GetSrv(18), graphics->GetSrvGpu(18), graphics->GetDsv(1));
}

void LightResourceManager::Update()
{
	FrameResource* currFrameResource = Scene::scene->frameResourceManager.currFrameResource;

	// Only the first "main" light casts a shadow.
	Vector3 lightDir = Graphics::Instance()->rotatedLightDirections[0];
	Vector3 center; center.xmf3 = sceneBounds.Center;
	Vector3 lightPos = (lightDir * (-2.0f * sceneBounds.Radius)) + center;
	Vector3 targetPos;
	targetPos.xmf3 = sceneBounds.Center;

	Vector3 lightUp{ 0.0f, 1.0f, 0.0f };
	Matrix4x4 lightView = Matrix4x4::MatrixLookAtLH(lightPos, targetPos, lightUp);

	Vector3 lightPosW = lightPos;

	// Transform bounding sphere to light space.
	Vector3 sphereCenterLS = targetPos.TransformCoord(lightView);

	auto& mSceneBounds = sceneBounds;
	float l = (sphereCenterLS.x - mSceneBounds.Radius);
	float b = (sphereCenterLS.y - mSceneBounds.Radius);
	float n = (sphereCenterLS.z - mSceneBounds.Radius);
	float r = (sphereCenterLS.x + mSceneBounds.Radius);
	float t = sphereCenterLS.y + mSceneBounds.Radius;
	float f = sphereCenterLS.z + mSceneBounds.Radius;

	Matrix4x4 lightProj = Matrix4x4::MatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	Matrix4x4 T{
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f };

	Matrix4x4 S = lightView * lightProj * T;

	Matrix4x4 f4x4lightView = lightView;;
	Matrix4x4 f4x4lightProj = lightProj;
	Matrix4x4 f4x4shadowTransform = S;

	// UpdateShadowPassCB
	PassConstants mShadowPassCB;
	{
		Matrix4x4 view = f4x4lightView;
		Matrix4x4 proj = f4x4lightProj;
		Matrix4x4 viewProj = view * proj;

		Matrix4x4 invView = view.Inverse();
		Matrix4x4 invProj = proj.Inverse();
		Matrix4x4 invViewProj = viewProj.Inverse();

		UINT w = shadowMap->Width();
		UINT h = shadowMap->Height();

		mShadowPassCB.View = view.Transpose();
		mShadowPassCB.InvView = invView.Transpose();
		mShadowPassCB.Proj = proj.Transpose();
		mShadowPassCB.InvProj = invProj.Transpose();
		mShadowPassCB.ViewProj = viewProj.Transpose();
		mShadowPassCB.InvViewProj = invViewProj.Transpose();
		mShadowPassCB.EyePosW = lightPosW;
		mShadowPassCB.RenderTargetSize = Vector2((float)w, (float)h);
		mShadowPassCB.InvRenderTargetSize = Vector2(1.0f / w, 1.0f / h);
		mShadowPassCB.NearZ = sphereCenterLS.z - sceneBounds.Radius;
		mShadowPassCB.FarZ = sphereCenterLS.z + sceneBounds.Radius;

		currFrameResource->PassCB->CopyData(1, mShadowPassCB);
	}
}
