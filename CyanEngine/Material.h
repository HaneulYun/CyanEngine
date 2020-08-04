#pragma once

struct Material
{
	std::string Name;

	int MatCBIndex = -1;
	int DiffuseSrvHeapIndex = -1;
	int NormalSrvHeapIndex = -1;
	int EmissiveSrvHeapIndex = -1;
	int MaskSrvHeapIndex = -1;

	int NumFramesDirty = NUM_FRAME_RESOURCES;

	Vector4 DiffuseAlbedo{ 1.0f, 1.0f, 1.0f, 1.0f };
	Vector3 FresnelR0{ 0.01f, 0.01f, 0.01f };
	float Roughness{ 0.25f };
	Matrix4x4 MatTransform = Matrix4x4::MatrixIdentity();
};