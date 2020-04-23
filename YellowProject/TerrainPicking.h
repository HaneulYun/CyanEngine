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
		XMFLOAT4X4 proj = Camera::main->projection;

		float vx = (+2.0f * sx / CyanFW::Instance()->GetWidth() - 1.0f) / proj(0, 0);
		float vy = (-2.0f * sy / CyanFW::Instance()->GetHeight() + 1.0f) / proj(1, 1);

		XMFLOAT3 rayOrigin{ 0.0f, 0.0f, 0.0f };
		XMFLOAT3 rayDir{ vx, vy, 1.0f };

		auto transform = Camera::main->gameObject->GetComponent<Transform>();
		auto vLookAt = transform->position + transform->forward.Normalized();
		XMVECTOR pos = XMLoadFloat3(&transform->position.xmf3);
		XMVECTOR lookAt = XMLoadFloat3(&vLookAt.xmf3);
		XMVECTOR up{ 0, 1, 0 };
		XMMATRIX view = XMMatrixLookAtLH(pos, lookAt, up);
		XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);

		Mesh* geo = static_cast<MeshFilter*>(terrain->meshFilter)->mesh;

		XMMATRIX world = XMLoadFloat4x4(&terrain->World);
		XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);

		XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

		rayOrigin = NS_Vector3::TransformCoord(rayOrigin, toLocal);
		rayDir = NS_Vector3::TransformNormal(rayDir, toLocal);

		rayDir = NS_Vector3::Normalize(rayDir);

		XMFLOAT3 planeNormal{ 0.0f, 1.0f, 0.0f };
		float dot = NS_Vector3::DotProduct(planeNormal, rayDir);

		XMFLOAT3 point;

		if (IntersectPlane(rayOrigin, rayDir, XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 1,0,0 }, XMFLOAT3{ 0,0,1 }))
		{
			point = NS_Vector3::Add(rayOrigin, NS_Vector3::ScalarProduct(rayDir, dT));
			GameObject* go{ prefab };
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
};