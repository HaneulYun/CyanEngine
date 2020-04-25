#include "pch.h"
#include "ParticleScene.h"

std::unique_ptr<Mesh> MakeParticle()
{
	auto mesh = std::make_unique<Mesh>();

	mesh->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	FrameResource::TreeSpriteVertex v[1];
	v[0].Pos = { 0, 0, 0 };

	uint16_t i[1]{ 0 };

	std::vector<FrameResource::TreeSpriteVertex> vertices;
	std::vector<std::uint16_t> indices;

	for (auto& d : v) vertices.push_back(d);
	for (auto& d : i) indices.push_back(d);

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(FrameResource::TreeSpriteVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	D3DCreateBlob(vbByteSize, &mesh->VertexBufferCPU);
	CopyMemory(mesh->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);
	D3DCreateBlob(ibByteSize, &mesh->IndexBufferCPU);
	CopyMemory(mesh->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	auto device = Graphics::Instance()->device;
	auto commandList = Graphics::Instance()->commandList;

	mesh->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(), commandList.Get(), vertices.data(), vbByteSize, mesh->VertexBufferUploader);
	mesh->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(), commandList.Get(), indices.data(), ibByteSize, mesh->IndexBufferUploader);

	mesh->VertexByteStride = sizeof(FrameResource::TreeSpriteVertex);
	mesh->VertexBufferByteSize = vbByteSize;
	mesh->IndexFormat = DXGI_FORMAT_R16_UINT;
	mesh->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	mesh->DrawArgs["submesh"] = submesh;

	return std::move(mesh);
}

void ParticleScene::BuildObjects()
{
	///*** Asset ***///
	//*** Texture ***//
	AddTexture(0, "none", L"Textures\\none.dds");

	//*** Material ***//
	AddMaterial(0, "none", 0);

	//*** Mesh ***//
	geometries["Image"] = Mesh::CreateQuad();
	geometries["Sphere"] = Mesh::CreateSphere();
	geometries["Plane"] = Mesh::CreatePlane();
	geometries["Quad"] = Mesh::CreateQuad();
	geometries["Particle"] =  MakeParticle();

	///*** Game Object ***///

	auto mainCamera = CreateEmpty();
	{
		camera = camera->main = mainCamera->AddComponent<Camera>();
		mainCamera->AddComponent<CameraController>();
	}

	auto skyBox = CreateEmpty();
	{
		skyBox->GetComponent<Transform>()->Scale({ 5000.0f, 5000.0f, 5000.0f });
		skyBox->AddComponent<Renderer>()->materials.push_back(1);
		auto mesh = skyBox->AddComponent<MeshFilter>()->mesh = geometries["Sphere"].get();
		renderObjectsLayer[(int)RenderLayer::Sky][mesh].gameObjects.push_back(skyBox);
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
		grid->AddComponent<MeshFilter>()->mesh = geometries["Plane"].get();
		grid->AddComponent<Renderer>()->materials.push_back(0);
		renderObjectsLayer[(int)RenderLayer::Opaque][geometries["Plane"].get()].gameObjects.push_back(grid);
	}

	auto particleSystemObject = CreateEmpty();
	{
		particleSystemObject->AddComponent<ParticleSystem>()->particle = geometries["Particle"].get();
		particleSystemObject->AddComponent<Renderer>()->materials.push_back(0);
		renderObjectsLayer[(int)RenderLayer::Particle][geometries["Particle"].get()].gameObjects.push_back(particleSystemObject);
	}
}
