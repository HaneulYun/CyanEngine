#include "pch.h"
#include "LightResourceManager.h"

void LightResourceManager::Update()
{
	for (int i = 0; i < Light::Type::Count; ++i)
	{
		for (auto light : lightObjects[i])
		{
			auto gameObject = light->gameObject;
			auto& shadowMap = light->shadowMap;

			auto camera = Camera::main;

			// Only the first "main" light casts a shadow.
			Vector3 lightDir = gameObject->transform->forward;
			Vector3 lightPos = gameObject->transform->position + camera->gameObject->transform->position;
			Vector3 targetPos = lightPos + lightDir * 1;

			Vector3 lightUp{ 0.0f, 1.0f, 0.0f };
			Matrix4x4 lightView = Matrix4x4::MatrixLookAtLH(lightPos, targetPos, lightUp);

			Vector3 lightPosW = lightPos;

			// Transform bounding sphere to light space.
			Vector3 targetPosC = targetPos.TransformCoord(lightView);

			float l = (targetPosC.x - 500) * 1;
			float r = (targetPosC.x + 500) * 1;
			float b = (targetPosC.y - 500) * 1;
			float t = (targetPosC.y + 500) * 1;
			float n = -1;
			float f = 1000;

			Matrix4x4 lightProj = Matrix4x4::MatrixOrthographicOffCenterLH(l, r, b, t, n, f);

			// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
			Matrix4x4 T{
				0.5f, 0.0f, 0.0f, 0.0f,
				0.0f, -0.5f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.5f, 0.5f, 0.0f, 1.0f };

			Matrix4x4 S = lightView * lightProj * T;

			// UpdateShadowPassCB
			PassConstants mShadowPassCB;
			{
				Matrix4x4 view = lightView;
				Matrix4x4 proj = lightProj;
				Matrix4x4 viewProj = view * proj;

				Matrix4x4 invView = view.Inverse();
				Matrix4x4 invProj = proj.Inverse();
				Matrix4x4 invViewProj = viewProj.Inverse();

				UINT w = shadowMap[0]->Width();
				UINT h = shadowMap[0]->Height();

				mShadowPassCB.View = view.Transpose();
				mShadowPassCB.InvView = invView.Transpose();
				mShadowPassCB.Proj = proj.Transpose();
				mShadowPassCB.InvProj = invProj.Transpose();
				mShadowPassCB.ViewProj = viewProj.Transpose();
				mShadowPassCB.InvViewProj = invViewProj.Transpose();
				mShadowPassCB.EyePosW = lightPosW;
				mShadowPassCB.RenderTargetSize = Vector2((float)w, (float)h);
				mShadowPassCB.InvRenderTargetSize = Vector2(1.0f / w, 1.0f / h);
				mShadowPassCB.NearZ = targetPosC.z - 1000;
				mShadowPassCB.FarZ = targetPosC.z + 1000;

				light->shadowTransform = S.Transpose();

				light->frameResources[Scene::scene->frameResourceManager.currFrameResourceIndex]->PassCB->CopyData(0, mShadowPassCB);
			}
		}
	}
}

void LightResourceManager::AddGameObject(GameObject* gameObject, int layer)
{
	auto graphics = Graphics::Instance();

	LightData* lightData = new LightData();
	lightData->gameObject = gameObject;
	if (!lightData->shadowMap.size())
	{
		for (int i = 0; i < 4; ++i)
		{
			lightData->shadowMap.push_back(std::make_unique<ShadowMap>(graphics->device.Get(), 2048, 2048));
			lightData->shadowMap[i]->BuildDescriptors(graphics->GetSrv(15+i), graphics->GetSrvGpu(15+i), graphics->GetDsv(1+i));
		}
	}


	for (int i = 0; i < NUM_FRAME_RESOURCES; ++i)
		lightData->frameResources.push_back(std::make_unique<FrameResource>(Graphics::Instance()->device.Get(), 1));


	lightObjects[layer].push_back(lightData);
}