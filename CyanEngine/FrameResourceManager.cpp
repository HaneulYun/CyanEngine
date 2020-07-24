#include "pch.h"
#include "FrameResourceManager.h"

void FrameResourceManager::Update()
{
	currFrameResourceIndex = (currFrameResourceIndex + 1) % NumFrameResources;
	currFrameResource = frameResources[currFrameResourceIndex].get();

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

		float mSunTheta = 1.25f * XM_PI;
		float mSunPhi = XM_PIDIV4;

		passConstants.EyePosW = pos;
		passConstants.AmbientLight = { 0.15f, 0.15f, 0.15f, 1.0f };

		passConstants.RenderTargetSize.x = CyanFW::Instance()->GetWidth();
		passConstants.RenderTargetSize.y = CyanFW::Instance()->GetHeight();

		passConstants.DeltaTime = Time::deltaTime;
		passConstants.TotalTime = Time::currentTime;
		currFrameResource->PassCB->CopyData(0, passConstants);
	}
}
