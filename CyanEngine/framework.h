#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <math.h>

#include <string>
#include <wrl.h>
#include <shellapi.h>

#include <typeinfo>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>

#include <d2d1_3.h>
#include <dwrite.h>
#include <d3d11on12.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#include <fbxsdk.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

using Microsoft::WRL::ComPtr;

//#define _WITH_TERRAIN_TESSELATION

#ifdef _WITH_TERRAIN_TESSELATION
#define _WITH_TERRAIN_PARTITION
#endif

#pragma comment(lib, "libfbxsdk-md.lib")
#pragma comment(lib, "libxml2-md.lib")
#pragma comment(lib, "zlib-md.lib")

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dxgi.lib")

#define FRAME_BUFFER_WIDTH 1600
#define FRAME_BUFFER_HEIGHT 900
#define PI (3.141592f)

//#define _WITH_SWAPCHAIN_FULLSCREEN_STATE

ID3D12Resource* CreateBufferResource(void* pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType = D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, ID3D12Resource** ppd3dUploadBuffer = NULL);
ID3D12Resource* CreateTextureResourceFromFile(const wchar_t* pszFileName, ID3D12Resource** ppd3dUploadBuffer, D3D12_RESOURCE_STATES d3dResourceStates);
ID3D12Resource* CreateTexture2DResource(UINT nWidth, UINT nHeight, UINT nElements, UINT nMipLevels, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue);

#include <deque>
#include <queue>
#include <map>

#define NUM_FRAME_RESOURCES 3

namespace NS_Vector4
{
	inline XMFLOAT4 Multiply(float fScalar, XMFLOAT4& xmf4Vector)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, fScalar * XMLoadFloat4(&xmf4Vector));
		return(xmf4Result);
	}
	inline XMFLOAT4 Add(const XMFLOAT4& xmf4Vector1, XMFLOAT4& xmf4Vector2)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) + XMLoadFloat4(&xmf4Vector2));
		return(xmf4Result);
	}
}

#include "d3dx12.h"
#include "d3dUtil.h"
#include "MathHelper.h"
#include "UploadBuffer.h"
#include "GeometryGenerator.h"

#include "Singleton.h"

#include "Vector3.h"
#include "Matrix4x4.h"

#include "Debug.h"
#include "Time.h"
#include "Input.h"
#include "Random.h"


#include "Object.h"

#include "Mesh.h"
#include "Meshs.h"

#include "Material.h"

#include "Component.h"
#include "MonoBehavior.h"

#include "Transform.h"
#include "MeshFilter.h"
#include "Renderer.h"
#include "SkinnedMeshRenderer.h"
#include "Camera.h"
#include "Terrain.h"

#include "AnimatorController.h"
#include "Animator.h"

#include "Collider.h"
#include "BoxCollider.h"
#include "SphereCollider.h"

#include "RectTransform.h"
#include "Image.h"
#include "Text.h"
#include "Button.h"

#include "Graphics.h"

#include "GameObject.h"

#include "FbxLoader.h"
#include "Scene.h"
#include "SceneManager.h"

#include "CyanFW.h"
#include "CyanApp.h"


#define RANDOM_COLOR XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX))

#define MAX_LIGHTS			8
#define MAX_MATERIALS		8
#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3