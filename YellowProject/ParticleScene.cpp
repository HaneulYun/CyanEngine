#include "pch.h"
#include "ParticleScene.h"

void ParticleScene::BuildObjects()
{
	///*** Asset ***///
	//*** Texture ***//
	//ASSET AddTexture("none", L"Textures\\none.dds");

	//*** Material ***//
	//ASSET AddMaterial("none", ASSET TEXTURE("none"));
	
	//*** Mesh ***//
	ASSET AddMesh("Plane", Mesh::CreatePlane());
	ASSET AddMesh("Quad", Mesh::CreateQuad());
	
	///*** Game Object ***///
	
	auto mainCamera = CreateEmpty();
	{
		camera = camera->main = mainCamera->AddComponent<Camera>();
		mainCamera->AddComponent<CameraController>();
	}
	
	auto skyBox = CreateEmpty();
	{
		skyBox->GetComponent<Transform>()->Scale({ 5000.0f, 5000.0f, 5000.0f });
		skyBox->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("none"));
		skyBox->AddComponent<MeshFilter>()->mesh = ASSET MESH("Sphere");
		skyBox->layer = (int)RenderLayer::Sky;
	}
	
	auto menuSceneButton = CreateImage();
	{
		auto rectTransform = menuSceneButton->GetComponent<RectTransform>();
		rectTransform->anchorMin = { 0, 1 };
		rectTransform->anchorMax = { 0, 1 };
		rectTransform->pivot = { 0, 1 };
		rectTransform->posX = 10;
		rectTransform->posY = -10;
		rectTransform->width = 150;
		rectTransform->height = 30;
	
		menuSceneButton->AddComponent<Button>()->AddEvent(
			[](void*) {
				SceneManager::LoadScene("MenuScene");
			});
		{
			auto textobject = menuSceneButton->AddChildUI();
			auto rectTransform = textobject->GetComponent<RectTransform>();
			rectTransform->anchorMin = { 0, 0 };
			rectTransform->anchorMax = { 1, 1 };
	
			Text* text = textobject->AddComponent<Text>();
			text->text = L"Menu Scene";
			text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
			text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			textObjects.push_back(textobject);
		}
	}
	
	auto grid = CreateEmpty();
	{
		grid->AddComponent<MeshFilter>()->mesh = ASSET MESH("Plane");
		grid->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("none"));
	}
	
	auto particleSystemObject = CreateEmpty();
	{
		particleSystemObject->AddComponent<ParticleSystem>()->Set();
		particleSystemObject->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("none"));
		particleSystemObject->layer = (int)RenderLayer::Particle;
	}
}
