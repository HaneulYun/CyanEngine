#include "pch.h"
#include "DetailPrototype.h"

void DetailPrototype::Set(TerrainData* terrainData)
{
	if (!ASSET MESH("Grass"))
	{

		// billboard points
		struct TreeSpriteVertex
		{
			XMFLOAT3 Pos;
			XMFLOAT2 Size;
			XMFLOAT3 look;
			XMFLOAT3 normal;
		};

		std::vector<TreeSpriteVertex> vertices;
		float sizex = 0.25, sizey = 0.25;
		const int width = terrainData->heightmapWidth - 1, length = terrainData->heightmapHeight - 1;
		float stride = 0.5f;
		vertices.reserve(width * length);
		for (float i = 0; i < width; i += stride)
		{
			for (float j = 0; j < length; j += stride)
			{
				TreeSpriteVertex v;
				float h = terrainData->GetHeight(i, j) * (terrainData->size.y / 255.0f);
				//if (h > 33.0f)
				//	continue;
				//if (h > 30.0f)
				//	v.Size = XMFLOAT2(sizex / (h - 30.0f), sizey / (h - 30.0f));
				//else
				v.Size = XMFLOAT2(sizex, sizey);
				v.Pos = XMFLOAT3(i, (terrainData->GetHeight(i, j) + sizey / 2) * (terrainData->size.y / 255.0f), j);;
				v.look = XMFLOAT3(MathHelper::RandF(-1.0f, 1.0f), 0.0f, MathHelper::RandF(-1.0f, 1.0f));
				v.normal = terrainData->GetHeightMapNormal(i, j).xmf3;
				vertices.push_back(v);
			}
		}

		auto geo = std::make_unique<Mesh>();
		const UINT vbByteSize = (UINT)vertices.size() * sizeof(TreeSpriteVertex);

		geo->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU);
		CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

		auto device = Graphics::Instance()->device;
		auto commandList = Graphics::Instance()->commandList;

		geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(), commandList.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);

		geo->VertexByteStride = sizeof(TreeSpriteVertex);
		geo->VertexBufferByteSize = vbByteSize;

		SubmeshGeometry submesh;
		submesh.IndexCount = vertices.size();
		submesh.StartIndexLocation = 0;
		submesh.BaseVertexLocation = 0;

		geo->DrawArgs["submesh"] = submesh;

		ASSET AddMesh("Grass", std::move(geo));
	}

	mesh = ASSET MESH("Grass");
	material = ASSET MATERIAL("grass");

	//{
	//	GameObject* billboards = CreateEmpty();
	//	billboards->GetComponent<Transform>()->position -= {128, 10, 128};
	//	auto mesh = billboards->AddComponent<MeshFilter>()->mesh = ASSET MESH("Grass");
	//	billboards->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("grass"));
	//	billboards->layer = (int)RenderLayer::Grass;
	//}
}