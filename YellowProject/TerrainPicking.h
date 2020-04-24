#pragma once
#include "..\CyanEngine\framework.h"

class TerrainPicking : public MonoBehavior<TerrainPicking>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:
	GameObject* terrain;
	GameObject* prefab;
	float dT;

	CHeightMapImage* heightMap;
	CHeightMapGridMesh* mesh;

private:
	friend class GameObject;
	friend class MonoBehavior<TerrainPicking>;
	TerrainPicking() = default;
	TerrainPicking(TerrainPicking&) = default;

public:
	~TerrainPicking() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
		if (Input::GetMouseButtonUp(2))
		{
			Pick(Input::mousePosition.x, Input::mousePosition.y);
		}
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.

	void Pick(int sx, int sy)
	{
		Vector3 screenPos{ (float)sx, (float)sy, 1.0f };

		XMFLOAT3 rayOrigin{ 0.0f, 0.0f, 0.0f }; 
		XMFLOAT3 rayDir = Camera::main->ScreenToWorldPoint(screenPos).xmf3;

		XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(XMLoadFloat4x4(&Camera::main->view)), XMLoadFloat4x4(&Camera::main->view));
		XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(XMLoadFloat4x4(&terrain->transform->localToWorldMatrix)), XMLoadFloat4x4(&terrain->transform->localToWorldMatrix));
		XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

		rayOrigin = NS_Vector3::TransformCoord(rayOrigin, toLocal);
		rayDir = NS_Vector3::TransformNormal(rayDir, invWorld);

		rayDir = NS_Vector3::Normalize(rayDir);

		if (IntersectPlane(rayOrigin, rayDir, XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 1,0,0 }, XMFLOAT3{ 0,0,1 }))
		{
			std::vector<XMFLOAT3> vertices;
			XMFLOAT3 point;// = NS_Vector3::Add(rayOrigin, NS_Vector3::ScalarProduct(rayDir, dT));

			// origin ���κ��� dT ���̿� �ִ� ���� ����
			for (int i = 1; i < dT + 1; ++i)
			{
				point = NS_Vector3::Add(rayOrigin, NS_Vector3::ScalarProduct(rayDir, i));

				float x, y, z;
				x = std::floor(point.x);
				z = std::floor(point.z);
				y = mesh->OnGetHeight(x, z, heightMap);
				vertices.push_back({ x,y,z });

				x = std::ceil(point.x);
				z = std::ceil(point.z);
				y = mesh->OnGetHeight(x, z, heightMap);
				vertices.push_back({ x,y,z });

				if (point.x > point.z)
				{
					x = std::ceil(point.x);
					z = std::floor(point.z);
				}
				else
				{
					x = std::floor(point.x);
					z = std::ceil(point.z);
				}
				y = mesh->OnGetHeight(x, z, heightMap);
				vertices.push_back({ x,y,z });

			}
			IntersectVertices(rayOrigin, rayDir, vertices);
			point = NS_Vector3::Add(rayOrigin, NS_Vector3::ScalarProduct(rayDir, dT));
			point = NS_Vector3::TransformCoord(point, terrain->transform->localToWorldMatrix);

			GameObject* go = Scene::scene->Duplicate(prefab);
			Scene::scene->AddGameObject(go);
			go->transform->position = { point.x, point.y, point.z };
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

	void IntersectVertices(XMFLOAT3 rayOrigin, XMFLOAT3 rayDirection, std::vector<XMFLOAT3>& vertices)
	{
		UINT triCount = vertices.size() / 3;

		float tmin = MathHelper::Infinity;
		for (UINT i = 0; i < triCount; ++i)
		{
			XMVECTOR v0 = XMLoadFloat3(&vertices[i * 3 + 0]);
			XMVECTOR v1 = XMLoadFloat3(&vertices[i * 3 + 1]);
			XMVECTOR v2 = XMLoadFloat3(&vertices[i * 3 + 2]);

			float t = 0.0f;
			if (TriangleTests::Intersects(XMLoadFloat3(&rayOrigin), XMLoadFloat3(&rayDirection), v0, v1, v2, t))
			{
				if (t < tmin)
				{
					tmin = t;
					dT = tmin;
				}
			}
		}
	}
};