#include "pch.h"
#include "FrameResourceManager.h"

void FrameResourceManager::Update()
{
	currFrameResourceIndex = (currFrameResourceIndex + 1) % NumFrameResources;
	currFrameResource = frameResources[currFrameResourceIndex].get();

	//// Animate the lights
	//Graphics::Instance()->lightRotationAngle += 0.1f * Time::deltaTime;
	//
	//Matrix4x4 R = Matrix4x4::RotationY(Graphics::Instance()->lightRotationAngle);
	//for (int i = 0; i < 3; ++i)
	//{
	//	Graphics::Instance()->rotatedLightDirections[i] = Graphics::Instance()->baseLightDirections[i].TransformNormal(R);
	//}
	//
	//// Update ShadowTransform
	//
	//// Only the first "main" light casts a shadow.
	//Vector3 lightDir = Graphics::Instance()->rotatedLightDirections[0];
	//Vector3 center; center.xmf3 = Graphics::Instance()->sceneBounds.Center;
	//Vector3 lightPos = (lightDir * (-2.0f * Graphics::Instance()->sceneBounds.Radius)) + center;
	//Vector3 targetPos; targetPos.xmf3 = Graphics::Instance()->sceneBounds.Center;
	//Vector3 lightUp{ 0.0f, 1.0f, 0.0f };
	//Matrix4x4 lightView = Matrix4x4::MatrixLookAtLH(lightPos, targetPos, lightUp);
	//
	//Vector3 lightPosW = lightPos;
	//
	//// Transform bounding sphere to light space.
	//Vector3 sphereCenterLS = targetPos.TransformCoord(lightView);
	//
	//auto& mSceneBounds = Graphics::Instance()->sceneBounds;
	//float l = (sphereCenterLS.x - mSceneBounds.Radius);
	//float b = (sphereCenterLS.y - mSceneBounds.Radius);
	//float n = (sphereCenterLS.z - mSceneBounds.Radius);
	//float r = (sphereCenterLS.x + mSceneBounds.Radius);
	//float t = sphereCenterLS.y + mSceneBounds.Radius;
	//float f = sphereCenterLS.z + mSceneBounds.Radius;
	//
	//Matrix4x4 lightProj = Matrix4x4::MatrixOrthographicOffCenterLH(l, r, b, t, n, f);
	//
	//// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	//Matrix4x4 T{
	//	0.5f, 0.0f, 0.0f, 0.0f,
	//	0.0f, -0.5f, 0.0f, 0.0f,
	//	0.0f, 0.0f, 1.0f, 0.0f,
	//	0.5f, 0.5f, 0.0f, 1.0f };
	//
	//Matrix4x4 S = lightView * lightProj * T;
	//
	//Matrix4x4 f4x4lightView = lightView;;
	//Matrix4x4 f4x4lightProj = lightProj;
	//Matrix4x4 f4x4shadowTransform = S;
	//
	//// UpdateShadowPassCB
	//PassConstants mShadowPassCB;
	//{
	//	Matrix4x4 view = f4x4lightView;
	//	Matrix4x4 proj = f4x4lightProj;
	//	Matrix4x4 viewProj = view * proj;
	//
	//	Matrix4x4 invView = view.Inverse();
	//	Matrix4x4 invProj = proj.Inverse();
	//	Matrix4x4 invViewProj = viewProj.Inverse();
	//
	//	UINT w = Graphics::Instance()->shadowMap->Width();
	//	UINT h = Graphics::Instance()->shadowMap->Height();
	//
	//	mShadowPassCB.View = view.Transpose();
	//	mShadowPassCB.InvView = invView.Transpose();
	//	mShadowPassCB.Proj = proj.Transpose();
	//	mShadowPassCB.InvProj = invProj.Transpose();
	//	mShadowPassCB.ViewProj = viewProj.Transpose();
	//	mShadowPassCB.InvViewProj = invViewProj.Transpose();
	//	mShadowPassCB.EyePosW = lightPosW;
	//	mShadowPassCB.RenderTargetSize = Vector2((float)w, (float)h);
	//	mShadowPassCB.InvRenderTargetSize = Vector2(1.0f / w, 1.0f / h);
	//	mShadowPassCB.NearZ = sphereCenterLS.z - Graphics::Instance()->sceneBounds.Radius;
	//	mShadowPassCB.FarZ = sphereCenterLS.z + Graphics::Instance()->sceneBounds.Radius;
	//
	//	currFrameResource->PassCB->CopyData(1, mShadowPassCB);
	//}

	{
		PassConstants passConstants;

		auto matrix = Scene::scene->camera->gameObject->GetMatrix();
		auto vLookAt = matrix.position + matrix.forward.Normalized();

		Vector3 pos = matrix.position;
		Vector3 lookAt = vLookAt;;
		Vector3 up{ 0, 1, 0 };
		Matrix4x4 view = Matrix4x4::MatrixLookAtLH(pos, lookAt, up);
		Matrix4x4 proj = Scene::scene->camera->projection;
		Matrix4x4 ViewProj = view * proj;

		passConstants.View = view.Transpose();
		passConstants.InvView = view.Inverse().Transpose();
		passConstants.Proj = proj.Transpose();
		passConstants.InvProj = proj.Inverse().Transpose();
		passConstants.ViewProj = ViewProj.Transpose();
		//passConstants.ShadowTransform = f4x4shadowTransform.Transpose();

		float mSunTheta = 1.25f * XM_PI;
		float mSunPhi = XM_PIDIV4;

		passConstants.EyePosW = pos;
		passConstants.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
		//passConstants.Lights[0].Direction = Graphics::Instance()->rotatedLightDirections[0];// { 0.57735f, -0.57735f, 0.57735f };
		passConstants.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
		passConstants.Lights[0].Strength = { 0.9f, 0.8f, 0.7f };
		//passConstants.Lights[1].Direction = Graphics::Instance()->rotatedLightDirections[1];// { -0.57735f, -0.57735f, 0.57735f };
		passConstants.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
		passConstants.Lights[1].Strength = { 0.4f, 0.4f, 0.4f };
		//passConstants.Lights[2].Direction = Graphics::Instance()->rotatedLightDirections[2];// { 0.0f, -0.707f, -0.707f };
		passConstants.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
		passConstants.Lights[2].Strength = { 0.2f, 0.2f, 0.2f };

		passConstants.RenderTargetSize.x = CyanFW::Instance()->GetWidth();
		passConstants.RenderTargetSize.y = CyanFW::Instance()->GetHeight();

		passConstants.DeltaTime = Time::deltaTime;
		passConstants.TotalTime = Time::currentTime;
		currFrameResource->PassCB->CopyData(0, passConstants);
	}
}
