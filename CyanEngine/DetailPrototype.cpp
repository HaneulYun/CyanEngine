#include "pch.h"
#include "DetailPrototype.h"

void DetailPrototype::Set(TerrainData* terrainData)
{
	// billboard points
	struct TreeSpriteVertex
	{
		XMFLOAT3 Pos;
		XMFLOAT2 Size;
		XMFLOAT3 look;
	};
	std::vector<TreeSpriteVertex> vertices;
	float sizex = 2, sizey = 2;
	const int width = 256, length = 256;
	vertices.reserve(width * length);
	
	auto gridMesh = terrainData->heightmapTexture;
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < length; ++j)
		{
			TreeSpriteVertex v;
			v.Pos = XMFLOAT3(i, gridMesh->OnGetHeight(i, j, terrainData) + sizey / 2, j);
			v.Size = XMFLOAT2(sizex, sizey);
			v.look = XMFLOAT3(MathHelper::RandF(0.0f, 1.0f), 0.0f, MathHelper::RandF(0.0f, 1.0f));
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