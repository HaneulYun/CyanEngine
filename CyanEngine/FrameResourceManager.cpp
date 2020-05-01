#include "pch.h"
#include "FrameResourceManager.h"

void FrameResourceManager::Update()
{
	currFrameResourceIndex = (currFrameResourceIndex + 1) % NumFrameResources;
	currFrameResource = frameResources[currFrameResourceIndex].get();

	// UpdateMaterialBuffer
	auto currMaterialBuffer = currFrameResource->MaterialBuffer.get();
	for (auto& e : scene->materials)
	{
		Material* mat = e.second.get();
		if (mat->NumFramesDirty > 0)
		{
			Matrix4x4 matTransform = mat->MatTransform;

			MaterialData matData;
			matData.DiffuseAlbedo = mat->DiffuseAlbedo;
			matData.FresnelR0 = mat->FresnelR0;
			matData.Roughness = mat->Roughness;
			matData.MatTransform = matTransform.Transpose();
			matData.DiffuseMapIndex = mat->DiffuseSrvHeapIndex;

			currMaterialBuffer->CopyData(mat->MatCBIndex, matData);

			--mat->NumFramesDirty;
		}
	}
}
