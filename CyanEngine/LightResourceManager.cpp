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

			LightConstants mShadowPassCB;
			for (int i = 0; i < 4; ++i)
			{
				int range;
				switch (i)
				{
				case 0: range = 200; break;
				case 1: range = 50; break;
				case 2: range = 100; break;
				case 3: range = 200; break;
				}
				float l = targetPosC.x - range;
				float r = targetPosC.x + range;
				float b = targetPosC.y - range;
				float t = targetPosC.y + range;
				float n = -1;
				float f = range * 2 - 1;

				Matrix4x4 lightProj = Matrix4x4::MatrixOrthographicOffCenterLH(l, r, b, t, n, f);

				// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
				Matrix4x4 T{
					0.5f, 0.0f, 0.0f, 0.0f,
					0.0f, -0.5f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.5f, 0.5f, 0.0f, 1.0f };

				Matrix4x4 S = lightView * lightProj * T;
				light->shadowTransform[i] = S.Transpose();

				Matrix4x4 viewProj = lightView * lightProj;
				mShadowPassCB.ViewProj[i] = viewProj.Transpose();
			}

			{
				UINT w = shadowMap[0]->Width();
				UINT h = shadowMap[0]->Height();

				mShadowPassCB.EyePosW = lightPosW;
				mShadowPassCB.RenderTargetSize = Vector2((float)w, (float)h);
				mShadowPassCB.InvRenderTargetSize = Vector2(1.0f / w, 1.0f / h);
				mShadowPassCB.NearZ = targetPosC.z - 1000;
				mShadowPassCB.FarZ = targetPosC.z + 1000;

				light->lightResource[Scene::scene->frameResourceManager.currFrameResourceIndex]->LightCB->CopyData(0, mShadowPassCB);
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
	{
		auto resource = std::make_unique<LightResource>();
		
		resource->LightCB = std::make_unique<UploadBuffer<LightConstants>>(Graphics::Instance()->device.Get(), 1, true);
		
		lightData->lightResource.push_back(std::move(resource));
	}

	lightObjects[layer].push_back(lightData);
}