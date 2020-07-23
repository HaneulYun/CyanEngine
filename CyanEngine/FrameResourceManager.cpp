#include "pch.h"
#include "FrameResourceManager.h"

void FrameResourceManager::Update()
{
	currFrameResourceIndex = (currFrameResourceIndex + 1) % NumFrameResources;
	currFrameResource = frameResources[currFrameResourceIndex].get();

	// Animate the lights
	Graphics::Instance()->lightRotationAngle += 0.1f * Time::deltaTime;
	Graphics::Instance()->baseLightDirections[0] = { 0,-1,0 };

	Matrix4x4 R = Matrix4x4::RotationZ(Graphics::Instance()->lightRotationAngle);
	for (int i = 0; i < 1; ++i)
	{
		Graphics::Instance()->rotatedLightDirections[i] = Graphics::Instance()->baseLightDirections[i].TransformNormal(R);
	}

	{
		PassConstants passConstants;

		auto matrix = Scene::scene->camera->gameObject->GetMatrix();
		auto vLookAt = matrix.position + Vector3::Normalize(matrix.forward);

		Vector3 pos = matrix.position;
		Vector3 lookAt = vLookAt;;
		Vector3 up{ 0, 1, 0 };
		Matrix4x4 view = Matrix4x4::MatrixLookAtLH(pos, lookAt, up);
		Matrix4x4 proj = Scene::scene->camera->projection;
		Matrix4x4 ViewProj = view * proj;

		Matrix4x4 f4x4shadowTransform = Matrix4x4::MatrixIdentity();
		passConstants.View = view.Transpose();
		passConstants.InvView = view.Inverse().Transpose();
		passConstants.Proj = proj.Transpose();
		passConstants.InvProj = proj.Inverse().Transpose();
		passConstants.ViewProj = ViewProj.Transpose();
		passConstants.ShadowTransform = f4x4shadowTransform.Transpose();

		float mSunTheta = 1.25f * XM_PI;
		float mSunPhi = XM_PIDIV4;

		passConstants.EyePosW = pos;
		passConstants.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
		passConstants.Lights[0].Direction = Graphics::Instance()->rotatedLightDirections[0];// { 0.57735f, -0.57735f, 0.57735f };
		//passConstants.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
		passConstants.Lights[0].Strength = { 0.9f, 0.8f, 0.7f };
		//passConstants.Lights[1].Direction = Graphics::Instance()->rotatedLightDirections[1];// { -0.57735f, -0.57735f, 0.57735f };
		passConstants.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
		passConstants.Lights[1].Strength = { 0, 0, 0 };
		//passConstants.Lights[2].Direction = Graphics::Instance()->rotatedLightDirections[2];// { 0.0f, -0.707f, -0.707f };
		passConstants.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
		passConstants.Lights[2].Strength = { 0, 0, 0 };

		passConstants.RenderTargetSize.x = CyanFW::Instance()->GetWidth();
		passConstants.RenderTargetSize.y = CyanFW::Instance()->GetHeight();

		passConstants.DeltaTime = Time::deltaTime;
		passConstants.TotalTime = Time::currentTime;
		currFrameResource->PassCB->CopyData(0, passConstants);
	}
}
