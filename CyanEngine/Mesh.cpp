#include "pch.h"
#include "Mesh.h"

void Mesh::Create(GeometryType type)
{
	std::vector<FrameResource::Vertex> vertices;
	std::vector<std::uint16_t> indices;
	{
		GeometryGenerator geoGen;
		GeometryGenerator::MeshData box;

		switch (type)
		{
		case GeometryType::GeometryType_Cube:
			box = geoGen.CreateBox(0.2f, 0.2f, 0.2f, 1); break;
		case GeometryType::GeometryType_Sphere:
			box = geoGen.CreateSphere(0.5f, 20, 20); break;
		case GeometryType::GeometryType_Cylinder:
			box = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20); break;
		case GeometryType::GeometryType_Plane:
			box = geoGen.CreateGrid(20.0f, 30.0f, 60, 40); break;
		case GeometryType::GeometryType_Quad:
			box = geoGen.CreateQuad(0, 1, 1, 1, 0); break;
		}

		vertices.resize(box.Vertices.size());
		for (size_t i = 0; i < box.Vertices.size(); ++i)
		{
			vertices[i].Pos = box.Vertices[i].Position;
			vertices[i].Normal = box.Vertices[i].Normal;
			vertices[i].TexC = box.Vertices[i].TexC;
		}
		indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
	}

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(FrameResource::Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);


	D3DCreateBlob(vbByteSize, &VertexBufferCPU);
	CopyMemory(VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);
	D3DCreateBlob(ibByteSize, &IndexBufferCPU);
	CopyMemory(IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	auto device = Graphics::Instance()->device;
	auto commandList = Graphics::Instance()->commandList;

	VertexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(), commandList.Get(), vertices.data(), vbByteSize, VertexBufferUploader);
	IndexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(), commandList.Get(), indices.data(), ibByteSize, IndexBufferUploader);

	VertexByteStride = sizeof(FrameResource::Vertex);
	VertexBufferByteSize = vbByteSize;
	IndexFormat = DXGI_FORMAT_R16_UINT;
	IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	DrawArgs["submesh"] = submesh;
}