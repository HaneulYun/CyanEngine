#pragma once
#include "..\CyanEngine\framework.h"

class TerrainPicking : public MonoBehavior<TerrainPicking>
{
private /*이 영역에 private 변수를 선언하세요.*/:

public  /*이 영역에 public 변수를 선언하세요.*/:
	GameObject* terrain;
	GameObject* prefab;
	float dT;

private:
	friend class GameObject;
	friend class MonoBehavior<TerrainPicking>;
	TerrainPicking() = default;
	TerrainPicking(TerrainPicking&) = default;

public:
	~TerrainPicking() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		if (Input::GetMouseButtonUp(2))
		{
			Pick(Input::mousePosition.x, Input::mousePosition.y);
		}
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.

	void Pick(int sx, int sy)
	{
		Vector3 screenPos{ (float)sx, (float)sy, 1.0f };

		XMFLOAT3 rayOrigin{ 0.0f, 0.0f, 0.0f }; 
		XMFLOAT3 rayDir = Camera::main->ScreenToWorldPoint(screenPos).xmf3;

		XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(XMLoadFloat4x4(&Camera::main->view)), XMLoadFloat4x4(&Camera::main->view));
		XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(XMLoadFloat4x4(&terrain->World)), XMLoadFloat4x4(&terrain->World));
		XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

		rayOrigin = NS_Vector3::TransformCoord(rayOrigin, toLocal);
		rayDir = NS_Vector3::TransformNormal(rayDir, invWorld);

		rayDir = NS_Vector3::Normalize(rayDir);

		//if (IntersectPlane(rayOrigin, rayDir, XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 1,0,0 }, XMFLOAT3{ 0,0,1 }))
		//{
		//	XMFLOAT3 point = NS_Vector3::Add(rayOrigin, NS_Vector3::ScalarProduct(rayDir, dT));
		//	point = NS_Vector3::TransformCoord(point, terrain->World);
		//	
		//	GameObject* go = Scene::scene->Duplicate(prefab);
		//	Scene::scene->AddGameObject(go);
		//	go->transform->position = { point.x, point.y,point.z };
		//	Mesh* mesh = static_cast<MeshFilter*>(go->meshFilter)->mesh;
		//	Scene::scene->renderObjectsLayer[(int)RenderLayer::Opaque][mesh].gameObjects.push_back(go);
		//}
		if (IntersectPlane(rayOrigin, rayDir, XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 1,0,0 }, XMFLOAT3{ 0,0,1 }))
		{
			//IntersectTri(rayOrigin, rayDir);

			XMFLOAT3 point = NS_Vector3::Add(rayOrigin, NS_Vector3::ScalarProduct(rayDir, dT));
			point = NS_Vector3::TransformCoord(point, terrain->World);



			GameObject* go = Scene::scene->Duplicate(prefab);
			Scene::scene->AddGameObject(go);
			go->transform->position = { point.x, point.y,point.z };
			Mesh* mesh = static_cast<MeshFilter*>(go->meshFilter)->mesh;
			Scene::scene->renderObjectsLayer[(int)RenderLayer::Opaque][mesh].gameObjects.push_back(go);
		}

	}

	bool IntersectPlane(XMFLOAT3 rayOrigin, XMFLOAT3 rayDirection, XMFLOAT3 v0, XMFLOAT3 v1, XMFLOAT3 v2)
	{
		XMFLOAT3 edge1{ v1.x - v0.x,v1.y - v0.y,v1.z - v0.z };
		XMFLOAT3 edge2{ v2.x - v0.x,v2.y - v0.y, v2.z - v0.z };

		XMFLOAT3 pvec = NS_Vector3::CrossProduct(rayDirection, edge2);
		
		float dot = NS_Vector3::DotProduct(edge1, pvec);

		if (dot > 0.0001f)
			return false;

		XMFLOAT3 planeNormal = NS_Vector3::CrossProduct(edge1, edge2);

		float dot1 = NS_Vector3::DotProduct(planeNormal, v0);
		float dot2 = NS_Vector3::DotProduct(planeNormal, rayOrigin);
		float dot3 = NS_Vector3::DotProduct(planeNormal, rayDirection);

		dT = (dot1 - dot2) / dot3;

		return true;
		
	}

	void IntersectTri(XMFLOAT3 rayOrigin, XMFLOAT3 rayDirection)
	{
		Mesh* geo = (static_cast<MeshFilter*>(terrain->meshFilter))->mesh;
		auto vertices = (FrameResource::Vertex*)geo->VertexBufferCPU->GetBufferPointer();
		auto indices = (UINT*)geo->IndexBufferCPU->GetBufferPointer();
		int a = (sizeof(UINT));
		UINT triCount = geo->IndexBufferByteSize / a - 2;

		float tmin = MathHelper::Infinity;
		for (UINT i = 0; i < triCount; ++i)
		{
			UINT i0;
			UINT i1;
			UINT i2;
			if (i % 2 == 0)
			{
				i0 = indices[i + 0];
				i1 = indices[i + 2];
				i2 = indices[i + 1];
			}
			else
			{
				i0 = indices[i + 0];
				i1 = indices[i + 1];
				i2 = indices[i + 2];
			}

			XMVECTOR v0 = XMLoadFloat3(&vertices[i0].Pos);
			XMVECTOR v1 = XMLoadFloat3(&vertices[i1].Pos);
			XMVECTOR v2 = XMLoadFloat3(&vertices[i2].Pos);

			float t = 0.0f;
			if (TriangleTests::Intersects(XMLoadFloat3(&rayOrigin), XMLoadFloat3(&rayDirection), v0, v1, v2, t))
			{
				if (t < tmin)
				{
					tmin = t;
				}
			}
		}
		dT = tmin;

	}
};