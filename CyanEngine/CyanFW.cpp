#include "pch.h"
#include "CyanFW.h"

CyanFW::CyanFW(UINT width, UINT height, std::wstring name)
	 : width(width), height(height), title(name)
{
	instance = this;
	_tcscpy_s(m_pszFrameRate, _T("CyanEngine ("));

	aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

CyanFW::~CyanFW()
{
}

bool CyanFW::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	Time::Instance()->Reset();
	Input::Instance();
	Random::Instance()->Start();

	if (!graphics)
		(graphics = Graphics::Instance())->Initialize();
	if (!sceneManager)
		sceneManager = SceneManager::Instance();
	if (!assetManager)
	{
		assetManager = AssetManager::Instance();
		for (int i = 0; i < NUM_FRAME_RESOURCES; ++i)
		{
			auto resource = std::make_unique<AssetResource>();
			resource->MaterialBuffer = std::make_unique<UploadBuffer<MaterialData>>(Graphics::Instance()->device.Get(), 21, false);
			assetManager->assetResource.push_back(std::move(resource));
		}
	}

	return true;
}

void CyanFW::OnFrameAdvance()
{
	Time::Instance()->Tick();

	if (sceneManager->nextScene)
	{
		sceneManager->nextScene->frameResourceManager.currFrameResourceIndex = sceneManager->scene->frameResourceManager.currFrameResourceIndex;
		Scene::scene = sceneManager->scene = sceneManager->nextScene;
		sceneManager->nextScene = nullptr;

		Camera::main = Scene::scene->camera;
	}

	if (sceneManager->scene->isDirty)
	{
		Graphics::Instance()->commandList->Reset(Graphics::Instance()->commandAllocator.Get(), nullptr);

		sceneManager->scene->isDirty = false;
		sceneManager->scene->Start();

		Graphics::Instance()->commandList->Close();
		ID3D12CommandList* cmdsLists[] = { Graphics::Instance()->commandList.Get() };
		Graphics::Instance()->commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	}

	sceneManager->scene->Update();

	// UpdateMaterialBuffer
	auto currMaterialBuffer = assetManager->assetResource[Scene::scene->frameResourceManager.currFrameResourceIndex]->MaterialBuffer.get();
	for (auto& e : AssetManager::Instance()->materials)
	{
		Material* mat = e.second.get();
		//if (mat->NumFramesDirty > 0)
		//{
			Matrix4x4 matTransform = mat->MatTransform;

			MaterialData matData;
			matData.DiffuseAlbedo = mat->DiffuseAlbedo;
			matData.FresnelR0 = mat->FresnelR0;
			matData.Roughness = mat->Roughness;
			matData.MatTransform = matTransform.Transpose();
			matData.DiffuseMapIndex = mat->DiffuseSrvHeapIndex;

			currMaterialBuffer->CopyData(mat->MatCBIndex, matData);

			--mat->NumFramesDirty;
		//}
	}

	graphics->Render();

	Input::Update();
}

void CyanFW::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		Input::mouses[0] = true;
		Input::mouseDown[0] = true;
		break;
	case WM_MBUTTONDOWN:
		Input::mouses[1] = true;
		Input::mouseDown[1] = true;
		break;
	case WM_RBUTTONDOWN:
		Input::mouses[2] = true;
		Input::mouseDown[2] = true;
		break;
	case WM_LBUTTONUP:
		Input::mouses[0] = false;
		Input::mouseUp[0] = true;
		break;
	case WM_MBUTTONUP:
		Input::mouses[1] = false;
		Input::mouseUp[1] = true;
		break;
	case WM_RBUTTONUP:
		Input::mouses[2] = false;
		Input::mouseUp[2] = true;
		break;
	case WM_MOUSEMOVE:
		Input::mousePosition = Vector3(LOWORD(lParam), HIWORD(lParam), 0);
		break;
	case WM_MOUSEWHEEL:
		Input::mouseWheel = GET_WHEEL_DELTA_WPARAM(wParam);
		break;
	}
}

void CyanFW::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case VK_OEM_PERIOD:
			Input::keys[(int)KeyCode::Period] = false;
			Input::keyUp[(int)KeyCode::Period] = true;
			break;
		case VK_F1:
			Graphics::Instance()->isShadowDebug = !Graphics::Instance()->isShadowDebug;
			break;
		case VK_F9:
			Graphics::Instance()->ChangeSwapChainState();
			break;
		}
		Input::keys[('a' <= wParam && wParam <= 'z') ? wParam - ('a' - 'A') : wParam] = false;
		Input::keyUp[('a' <= wParam && wParam <= 'z') ? wParam - ('a' - 'A') : wParam] = true;
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_OEM_PERIOD:
			Input::keys[(int)KeyCode::Period] = true;
			Input::keyDown[(int)KeyCode::Period] = true;
			break;
		}
		Input::keys[('a' <= wParam && wParam <= 'z') ? wParam - ('a' - 'A') : wParam] = true;
		Input::keyDown[('a' <= wParam && wParam <= 'z') ? wParam - ('a' - 'A') : wParam] = true;
		break;
	default:
		break;
	}
}

LRESULT CyanFW::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}

	return 0;
}
