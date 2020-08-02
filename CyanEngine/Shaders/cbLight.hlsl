cbuffer cbPass : register(b0)
{
	float4x4 gCameraProj;
	float4x4 gCameraView;
	float4x4 gCameraInvView;

	float4x4 gViewProj[4];
	float4x4 gViewProjS[4];

	float3 gEyePosW;
	float  cbPerObjectPad0;
	float3 gCameraEyePosW;
	uint   gCountShadowMap;
	float2 gRenderTargetSize;
	float2 gInvRenderTargetSize;

	float gNearZ;
	float gFarZ;
	float gTotalTime;
	float gDeltaTime;

	float4 gAmbientLight;
};