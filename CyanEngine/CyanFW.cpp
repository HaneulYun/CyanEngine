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
			resource->MaterialBuffer = std::make_unique<UploadBuffer<MaterialData>>(Graphics::Instance()->device.Get(), 20, false);
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

LRESULT CyanFW::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (!Input::ProcessingWindowMessage(hWnd, nMessageID, wParam, lParam))
		return 0;

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
	case WM_KEYDOWN:
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case VK_F9:
			Graphics::Instance()->ChangeSwapChainState();
			break;
		}
		break;
	}

	return 1;
}
