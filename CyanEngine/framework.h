#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#include <windows.h>

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

#include <fmod.hpp>

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

#ifdef _WIN64
#pragma comment(lib, "fmod64_vc.lib")
#else
#pragma comment(lib, "fmod_vc.lib")
#endif

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

#include <deque>
#include <queue>
#include <map>

#define NUM_FRAME_RESOURCES 3

#include "Vector3.h"
#include "Matrix4x4.h"


#include "d3dx12.h"
#include "d3dUtil.h"
#include "UploadBuffer.h"
#include "GeometryGenerator.h"


#include "Singleton.h"


#include "Debug.h"
#include "Time.h"
#include "Input.h"
#include "Random.h"


#include "Object.h"

#include "Texture.h"
#include "Material.h"

#include "Component.h"
#include "MonoBehavior.h"

#include "Transform.h"
#include "Renderer.h"
#include "Camera.h"
#include "Light.h"

#include "Mesh.h"
#include "SkinnedMeshRenderer.h"
#include "MeshFilter.h"

#include "DetailPrototype.h"
#include "TerrainData.h"
#include "RenderTexture.h"
#include "Terrain.h"

#include "Collider.h"
#include "BoxCollider.h"
#include "SphereCollider.h"

#include "GameObject.h"

#include "AnimatorController.h"
#include "Animator.h"

#include "RectTransform.h"
#include "Image.h"
#include "Text.h"
#include "Button.h"
#include "InputField.h"

#include "Constant.h"

#include "FrameResource.h"
#include "ParticleBundle.h"
#include "ParticleSystem.h"

#include "AudioManager.h"
#include "AudioSource.h"
#include "AudioListener.h"

#include "FbxLoader.h"

#include "AssetResource.h"
#include "ObjectResource.h"
#include "LightResource.h"

#include "HeapManager.h"
#include "Graphics.h"

#include "QuadNode.h"
#include "QuadTree.h"
#include "TagData.h"
#include "Sector.h"
#include "SpatialPartitioningManager.h"

#include "AssetManager.h"
#include "ObjectRenderManager.h"
#include "FrameResourceManager.h"
#include "LightResourceManager.h"
#include "Scene.h"

#include "SceneManager.h"
#include "CyanFW.h"
#include "CyanApp.h"


#define RANDOM_COLOR Vector4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX))

#define MAX_LIGHTS			8
#define MAX_MATERIALS		8
#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3