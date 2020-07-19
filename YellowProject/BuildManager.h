#pragma once
#include "..\CyanEngine\framework.h"

enum class BuildingType
{
	/*LandMark*/
	Well_01 = 0,

	/*House*/
	House_01 = 100, House_02,

	/*Theme*/

	/*landscape*/
	Tree_01 = 300, Tree_02, Tree_03, Tree_04, Tree_05,
	Tree_Stump_01, Tree_Stump_02,
	Plant_01, Flower_01, Flower_02, Flower_03, Flower_04, Flower_05, Flower_06,
	Grass_01, Grass_02, Grass_03,
	Mushroom_01, Mushroom_02, Mushroom_03, Mushroom_04, Mushroom_05, Mushroom_06,
	Stone_Big_01, Stone_Big_02, Stone_Big_03, Stone_Big_04, Stone_Big_05, Stone_Big_06,
	Stone_Medium_01, Stone_Medium_02, Stone_Medium_03, Stone_Medium_04, Stone_Medium_05,
	Stone_Small_01, Stone_Small_02, Stone_Small_03, Stone_Small_04, Stone_Small_05,
	Stone_Flat_01, Stone_Flat_02, Stone_Flat_03, Stone_Flat_04, Stone_Flat_05,
	StonePath_01, StonePath_02, StonePath_03,

	/*Decoration*/
	Fence_01 = 400, Fence_02, Fence_03, Fence_04,
	Street_Light_01,
	Bucket, Barrel, Pitchfork, Axe, Ladder, Spike, Cart_01, Cart_02,
	Torch_01, Torch_02,
	Logs_01, Logs_02, Log_01, Log_02, Log_03, Log_04,
	Pot_01, Pot_02, Pot_03, Pot_04, Pot_05, Pot_06, Cauldron, Crate_Open, Crate_Closed,
	Hay_Small_01, Hay_Small_02, Hay_Stack,
	Apple, Potato, Tomato, Fish, Watermellon,
	Sack_Apple, Sack_Flour, Sack_Potato, Sack_Tomato, Sack_01, Sack_02,
	Pumpkin_01, Pumpkin_02, Pumpkin_03
};

struct BuildingInform
{
	BuildingType buildingType;
	float xPos, yPos, zPos;
	float rotAngle;

	bool operator== (const BuildingInform& b) const
	{
		return ((xPos == b.xPos) && (yPos == b.yPos)
			&& (zPos == b.zPos));
	}

	friend std::ostream& operator<<(std::ostream& os, const BuildingInform& b);
	friend std::istream& operator>>(std::istream& in, BuildingInform& b);
};

static std::ostream& operator<<(std::ostream& os, const BuildingInform& b)
{
	os << static_cast<int>(b.buildingType) << " " << b.xPos << " " << b.yPos << " " << b.zPos << " " << b.rotAngle << std::endl;
	return os;
}

static std::istream& operator>>(std::istream& in, BuildingInform& b)
{
	int input;
	in >> input >> b.xPos >> b.yPos >> b.zPos >> b.rotAngle;
	b.buildingType = static_cast<BuildingType>(input);
	return in;
}

class BuildManager : public MonoBehavior<BuildManager>
{
private /*이 영역에 private 변수를 선언하세요.*/:
	//Mesh* model;
	GameObject* prefab{ nullptr };

public  /*이 영역에 public 변수를 선언하세요.*/:
	GameObject* terrain;
	TerrainData* heightMap;
	RenderTexture* terrainMesh;
	float dT;

	static BuildManager* buildManager;

private:
	friend class GameObject;
	friend class MonoBehavior<BuildManager>;
	BuildManager() = default;
	BuildManager(BuildManager&) = default;

public:
	~BuildManager() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		if (prefab)
			Pick(Input::mousePosition.x, Input::mousePosition.y);
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.

	void Pick(int sx, int sy)
	{
		Vector3 screenPos{ (float)sx, (float)sy, 1.0f };

		Vector3 rayOrigin{ 0.0f, 0.0f, 0.0f };
		Vector3 rayDir = Camera::main->ScreenToWorldPoint(screenPos);

		Matrix4x4 invView = Camera::main->view.Inverse();
		Matrix4x4 invWorld = terrain->transform->localToWorldMatrix.Inverse();
		Matrix4x4 toLocal = invView * invWorld;

		rayOrigin = rayOrigin.TransformCoord(toLocal);
		rayDir = rayDir.TransformNormal(invWorld);

		rayDir.Normalize();

		if (IntersectPlane(rayOrigin, rayDir, Vector3{ 0,0,0 }, Vector3{ 1,0,0 }, Vector3{ 0,0,1 }))
		{
			std::vector<XMFLOAT3> vertices;
			Vector3 point = rayOrigin + rayDir * dT;

			for (float i = 1; i < dT + 1; i += 0.5)
			{
				Vector3 p = rayOrigin + rayDir * i;
				if ((int)point.x == (int)p.x && (int)point.z == (int)p.z)
					continue;

				point = p;
				float x, y, z;
				if (std::ceil(point.x) > 1080 || std::ceil(point.z) > 1080)
					continue;
				if (std::ceil(point.x) < 0 || std::ceil(point.z) < 0)
					continue;

				x = std::floor(point.x);
				z = std::floor(point.z);
				vertices.push_back({ x,terrainMesh->OnGetHeight(x, z, heightMap),z });
				vertices.push_back({ x + 1,terrainMesh->OnGetHeight(x + 1, z + 1, heightMap),z + 1 });
				vertices.push_back({ x + 1,terrainMesh->OnGetHeight(x + 1, z, heightMap),z });

				vertices.push_back({ x,terrainMesh->OnGetHeight(x, z, heightMap),z });
				vertices.push_back({ x,terrainMesh->OnGetHeight(x, z + 1, heightMap),z + 1 });
				vertices.push_back({ x + 1,terrainMesh->OnGetHeight(x + 1, z + 1, heightMap),z + 1 });

			}
			IntersectVertices(rayOrigin.xmf3, rayDir.xmf3, vertices);
			point = rayOrigin + rayDir * dT;
			point = point.TransformCoord(terrain->transform->localToWorldMatrix);
			prefab->transform->position = { point.x, point.y + 1.0f, point.z };
			// 충돌되면 빨간색 쉐이더 사용

			if (Input::GetMouseButtonDown(0))
			{
				prefab->layer = (int)RenderLayer::Opaque;
				GameObject* go = Scene::scene->Duplicate(prefab);
				DeletePrefab();
			}
		}
	}

	bool IntersectPlane(Vector3 rayOrigin, Vector3 rayDirection, Vector3 v0, Vector3 v1, Vector3 v2)
	{
		Vector3 edge1{ v1.x - v0.x,v1.y - v0.y,v1.z - v0.z };
		Vector3 edge2{ v2.x - v0.x,v2.y - v0.y, v2.z - v0.z };

		Vector3 pvec = Vector3::CrossProduct(rayDirection, edge2);

		float dot = Vector3::DotProduct(edge1, pvec);

		if (dot > 0.0001f)
			return false;

		Vector3 planeNormal = Vector3::CrossProduct(edge1, edge2);

		float dot1 = Vector3::DotProduct(planeNormal, v0);
		float dot2 = Vector3::DotProduct(planeNormal, rayOrigin);
		float dot3 = Vector3::DotProduct(planeNormal, rayDirection);

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

			float x = (vertices[i * 3 + 0].x + vertices[i * 3 + 1].x + vertices[i * 3 + 2].x) / 3;
			float y = (vertices[i * 3 + 0].y + vertices[i * 3 + 1].y + vertices[i * 3 + 2].y) / 3;
			float z = (vertices[i * 3 + 0].z + vertices[i * 3 + 1].z + vertices[i * 3 + 2].z) / 3;
			XMMATRIX toLocal = XMMatrixTranslation(-x, -y, -z);
			XMMATRIX InvToLocal = XMMatrixInverse(&XMMatrixDeterminant(toLocal), toLocal);
			XMMATRIX s = XMMatrixScaling(1.2, 1.2, 1.2);

			v0 = XMVector3Transform(XMVector3Transform(XMVector3Transform(v0, toLocal), s), InvToLocal);
			v1 = XMVector3Transform(XMVector3Transform(XMVector3Transform(v1, toLocal), s), InvToLocal);
			v2 = XMVector3Transform(XMVector3Transform(XMVector3Transform(v2, toLocal), s), InvToLocal);

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

	void SelectModel(Mesh* mesh, Material* mat, float size)
	{
		if (prefab) {
			auto meshFilter = prefab->GetComponent<MeshFilter>();
			DeletePrefab();

			if (meshFilter && meshFilter->mesh == mesh)
				return;
		}
		// 프리팹 쉐이더 사용
		Scene::scene->CreateEmptyPrefab();
		prefab = Scene::scene->CreateEmpty();
		prefab->GetComponent<Transform>()->Scale({ size, size, size });
		prefab->AddComponent<MeshFilter>()->mesh = mesh;
		auto renderer = prefab->AddComponent<Renderer>();
		prefab->layer = (int)RenderLayer::BuildPreview;
		for (auto& sm : mesh->DrawArgs)
			renderer->materials.push_back(mat);
		prefab->AddComponent<Constant>()->v4 = { 0.0f,1.0f,0.0f,1.0f };
	}

	void DeletePrefab()
	{
		Scene::scene->PushDelete(prefab);
		prefab = nullptr;
	}

	void Load()
	{
		std::deque<BuildingInform> buildCommand;
		
		std::ifstream in("Buildings.txt");
		std::istream_iterator<BuildingInform> beg{ in };
		std::istream_iterator<BuildingInform> end{};
		
		while (beg != end)
		{
			buildCommand.emplace_back(*beg++);
		}

		while (!buildCommand.empty())
		{
			Mesh* mesh = NULL;
			Material* mat = NULL;
			float scaleSize = 0.01f;
			float colliderSize = NULL;

			BuildingInform frontInform = buildCommand.front();
			switch (frontInform.buildingType)
			{
			case BuildingType::House_01:
				mesh = ASSET MESH("SM_House_Var01");
				mat = ASSET MATERIAL("house01");
				colliderSize = 2.75f;
				break;
			case BuildingType::Mushroom_01:
				mesh = ASSET MESH("SM_Mushroom_Var01");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.2f;
				break;
			case BuildingType::Mushroom_02:
				mesh = ASSET MESH("SM_Mushroom_Var02");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.2f;
				break;
			case BuildingType::Mushroom_03:
				mesh = ASSET MESH("SM_Mushroom_Var03");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.2f;
				break;
			case BuildingType::Mushroom_04:
				mesh = ASSET MESH("SM_Mushroom_Var04");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.2f;
				break;
			case BuildingType::Mushroom_05:
				mesh = ASSET MESH("SM_Mushroom_Var05");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.15f;
				break;
			case BuildingType::Mushroom_06:
				mesh = ASSET MESH("SM_Mushroom_Var06");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.15f;
				break;
			case BuildingType::Bucket:
				mesh = ASSET MESH("SM_Bucket");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.4f;
				break;
			case BuildingType::Barrel:
				mesh = ASSET MESH("SM_Barrel");
				mat = ASSET MATERIAL("material_01");
				colliderSize = 0.75f;
				break;
			case BuildingType::Cauldron:
				mesh = ASSET MESH("SM_Cauldron");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.5f;
				break;
			case BuildingType::Pot_01:
				mesh = ASSET MESH("SM_Pot_Var01");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.4f;
				break;
			case BuildingType::Pot_02:
				mesh = ASSET MESH("SM_Pot_Var02");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.4f;
				break;
			case BuildingType::Pot_03:
				mesh = ASSET MESH("SM_Pot_Var03");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.4f;
				break;
			case BuildingType::Pot_04:
				mesh = ASSET MESH("SM_Pot_Var04");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.4f;
				break;
			case BuildingType::Pot_05:
				mesh = ASSET MESH("SM_Pot_Var05");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.4f;
				break;
			case BuildingType::Pot_06:
				mesh = ASSET MESH("SM_Pot_Var06");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.4f;
				break;
			case BuildingType::Apple:
				mesh = ASSET MESH("SM_Apple");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.1f;
				break;
			case BuildingType::Potato:
				mesh = ASSET MESH("SM_Potato");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.1f;
				break;
			case BuildingType::Tomato:
				mesh = ASSET MESH("SM_Tomato");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.15f;
				break;
			case BuildingType::Watermellon:
				mesh = ASSET MESH("SM_Watermellon");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.3f;
				break;
			case BuildingType::Sack_Apple:
				mesh = ASSET MESH("SM_Sack_Apple");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.5f;
				break;
			case BuildingType::Sack_Flour:
				mesh = ASSET MESH("SM_Sack_Flour");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.5f;
				break;
			case BuildingType::Sack_Potato:
				mesh = ASSET MESH("SM_Sack_Potato");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.5f;
				break;
			case BuildingType::Sack_Tomato:
				mesh = ASSET MESH("SM_Sack_Tomato");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.5f;
				break;
			case BuildingType::Sack_01:
				mesh = ASSET MESH("SM_Sack_Var01");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.5f;
				break;
			case BuildingType::Sack_02:
				mesh = ASSET MESH("SM_Sack_Var02");
				mat = ASSET MATERIAL("material_02");
				colliderSize = 0.4f;
				break;
			}

			switch (frontInform.buildingType)
			{
			case BuildingType::Well_01:
			{
				prefab = Scene::scene->CreateEmpty();
				//prefab->AddComponent<Building>();
				BoxCollider* collider = prefab->AddComponent<BoxCollider>();
				collider->extents = { 1.5f, 1.8f, 1.5f };
				collider->obb = true;
				{
					GameObject* child = prefab->AddChild();
					child->AddComponent<MeshFilter>()->mesh = ASSET MESH("SM_Well");
					child->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("material_03"));
					child->AddComponent<Constant>()->v4 = { 0.0f,1.0f,0.0f,1.0f };
					child->layer = (int)RenderLayer::Opaque;
					child->transform->Scale({ scaleSize, scaleSize, scaleSize });
					child->transform->Rotate({ 1.0,0.0,0.0 }, -90.0f);
				}
				{
					GameObject* child = prefab->AddChild();
					child->AddComponent<MeshFilter>()->mesh = ASSET MESH("SM_Well_Extra02");
					child->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("material_03"));
					child->AddComponent<Constant>()->v4 = { 0.0f,1.0f,0.0f,1.0f };
					child->layer = (int)RenderLayer::Opaque;
					child->transform->position = { 0.0f,1.5f,0.0f };
					child->transform->Scale({ scaleSize, scaleSize, scaleSize });
					child->transform->Rotate({ 1.0,0.0,0.0 }, -90.0f);
				}
				{
					GameObject* child = prefab->AddChild();
					child->AddComponent<MeshFilter>()->mesh = ASSET MESH("SM_Well_Extra03");
					child->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("material_03"));
					child->AddComponent<Constant>()->v4 = { 0.0f,1.0f,0.0f,1.0f };
					child->layer = (int)RenderLayer::Opaque;
					child->transform->position = { 0.0f,1.0f,0.0f };
					child->transform->Scale({ scaleSize, scaleSize, scaleSize });
					child->transform->Rotate({ 1.0,0.0,0.0 }, -90.0f);
				}
			}
			break;
			case BuildingType::House_02:
			{
				prefab = Scene::scene->CreateEmpty();
				//prefab->AddComponent<Building>();
				BoxCollider* collider = prefab->AddComponent<BoxCollider>();
				collider->extents = { 2.5f, 4.5f, 2.5f };
				collider->obb = true;
				{
					GameObject* child = prefab->AddChild();
					child->AddComponent<MeshFilter>()->mesh = ASSET MESH("SM_House_Var02");
					child->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("house02"));
					child->AddComponent<Constant>()->v4 = { 0.0f,1.0f,0.0f,1.0f };
					child->layer = (int)RenderLayer::Opaque;
					child->transform->Scale({ scaleSize, scaleSize, scaleSize });
					child->transform->Rotate({ 1.0,0.0,0.0 }, -90.0f);
				}
				{
					GameObject* child = prefab->AddChild();
					child->AddComponent<MeshFilter>()->mesh = ASSET MESH("SM_House_Var02_Extra");
					child->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("house02"));
					child->AddComponent<Constant>()->v4 = { 0.0f,1.0f,0.0f,1.0f };
					child->AddComponent<RotatingBehavior>()->speedRotating = -10.0f;
					child->layer = (int)RenderLayer::Opaque;
					child->transform->position = { 0.25f,6.0f,2.5f };
					child->transform->Scale({ scaleSize, scaleSize, scaleSize });
					child->transform->Rotate({ 1.0,0.0,0.0 }, -90.0f);
				}
			}
			break;
			case BuildingType::Fence_01:
				mesh = ASSET MESH("SM_Fence_Var01");
			case BuildingType::Fence_02:
				if (mesh == NULL) mesh = ASSET MESH("SM_Fence_Var02");
				mat = ASSET MATERIAL("material_01");
				{
					prefab = Scene::scene->CreateEmpty();
					//prefab->AddComponent<Building>();
					BoxCollider* collider = prefab->AddComponent<BoxCollider>();
					collider->extents = { 1.6f, 0.5f, 0.4f };
					collider->obb = true;

					GameObject* child = prefab->AddChild();
					child->AddComponent<MeshFilter>()->mesh = mesh;
					Renderer* renderer = child->AddComponent<Renderer>();
					if (mesh)
						for (auto& sm : mesh->DrawArgs)
							renderer->materials.push_back(mat);
					child->AddComponent<Constant>()->v4 = { 0.0f,1.0f,0.0f,1.0f };
					child->layer = (int)RenderLayer::Opaque;
					child->transform->Scale({ scaleSize, scaleSize, scaleSize });
					child->transform->Rotate({ 1.0,0.0,0.0 }, -90.0f);
				}
				break;
			case BuildingType::Fence_03:
				mesh = ASSET MESH("SM_Fence_Var03");
			case BuildingType::Fence_04:
				if (mesh == NULL) mesh = ASSET MESH("SM_Fence_Var04");
				mat = ASSET MATERIAL("material_01");
				{
					prefab = Scene::scene->CreateEmpty();
					//prefab->AddComponent<Building>();
					BoxCollider* collider = prefab->AddComponent<BoxCollider>();
					collider->extents = { 1.5f, 0.5f, 0.4f };
					collider->obb = true;

					GameObject* child = prefab->AddChild();
					child->AddComponent<MeshFilter>()->mesh = mesh;
					Renderer* renderer = child->AddComponent<Renderer>();
					if (mesh)
						for (auto& sm : mesh->DrawArgs)
							renderer->materials.push_back(mat);
					child->AddComponent<Constant>()->v4 = { 0.0f,1.0f,0.0f,1.0f };
					child->layer = (int)RenderLayer::Opaque;
					child->transform->Scale({ scaleSize, scaleSize, scaleSize });
					child->transform->Rotate({ 1.0,0.0,0.0 }, -90.0f);
				}
				break;
			case BuildingType::Spike:
				prefab = Scene::scene->CreateEmpty();
				//prefab->AddComponent<Building>();
				{
					prefab = Scene::scene->CreateEmpty();
					//prefab->AddComponent<Building>();
					{
						BoxCollider* collider = prefab->AddComponent<BoxCollider>();
						collider->extents = { 1.4f, 0.8f, 0.4f };
						collider->obb = true;
						mesh = ASSET MESH("SM_Spike");
						mat = ASSET MATERIAL("material_02");
						GameObject* child = prefab->AddChild();
						child->AddComponent<MeshFilter>()->mesh = mesh;
						Renderer* renderer = child->AddComponent<Renderer>();
						if (mesh)
							for (auto& sm : mesh->DrawArgs)
								renderer->materials.push_back(mat);
						child->AddComponent<Constant>()->v4 = { 0.0f,1.0f,0.0f,1.0f };
						child->layer = (int)RenderLayer::Opaque;
						child->transform->Scale({ scaleSize, scaleSize, scaleSize });
						child->transform->Rotate({ 1.0,0.0,0.0 }, -90.0f);
					}
					{
						mesh = ASSET MESH("SM_Fish");
						GameObject* child = prefab->AddChild();
						child->AddComponent<MeshFilter>()->mesh = mesh;
						Renderer* renderer = child->AddComponent<Renderer>();
						if (mesh)
							for (auto& sm : mesh->DrawArgs)
								renderer->materials.push_back(mat);
						child->AddComponent<Constant>()->v4 = { 0.0f,1.0f,0.0f,1.0f };
						child->layer = (int)RenderLayer::Opaque;
						child->transform->position = { -0.1f,0.75f,0.0f };
						child->transform->Scale({ scaleSize, scaleSize, scaleSize });
						child->transform->Rotate({ 1.0,0.0,0.0 }, -90.0f);
					}
				}
				break;
			case BuildingType::Cart_01:
				mesh = ASSET MESH("SM_Cart_Var01");
			case BuildingType::Cart_02:
				mesh = ASSET MESH("SM_Cart_Var02");
				mat = ASSET MATERIAL("material_01");
				{
					prefab = Scene::scene->CreateEmpty();
					//prefab->AddComponent<Building>();
					BoxCollider* collider = prefab->AddComponent<BoxCollider>();
					collider->extents = { 1.7f, 0.5f, 1.0f };
					collider->obb = true;

					GameObject* child = prefab->AddChild();
					child->AddComponent<MeshFilter>()->mesh = mesh;
					Renderer* renderer = child->AddComponent<Renderer>();
					if (mesh)
						for (auto& sm : mesh->DrawArgs)
							renderer->materials.push_back(mat);
					child->AddComponent<Constant>()->v4 = { 0.0f,1.0f,0.0f,1.0f };
					child->layer = (int)RenderLayer::Opaque;
					child->transform->Scale({ scaleSize, scaleSize, scaleSize });
					child->transform->Rotate({ 1.0,0.0,0.0 }, -90.0f);
				}
				break;
			default:
				prefab = Scene::scene->CreateEmpty();
				//prefab->AddComponent<Building>();
				prefab->AddComponent<BoxCollider>()->extents = { colliderSize, colliderSize, colliderSize };

				GameObject* child = prefab->AddChild();
				child->AddComponent<MeshFilter>()->mesh = mesh;
				Renderer* renderer = child->AddComponent<Renderer>();
				if (mesh)
					for (auto& sm : mesh->DrawArgs)
						renderer->materials.push_back(mat);
				child->AddComponent<Constant>()->v4 = { 0.0f,1.0f,0.0f,1.0f };
				child->layer = (int)RenderLayer::Opaque;
				child->transform->Scale({ scaleSize, scaleSize, scaleSize });
				child->transform->Rotate({ 1.0,0.0,0.0 }, -90.0f);
				break;
			}

			prefab->transform->position = { frontInform.xPos, frontInform.yPos, frontInform.zPos };
			prefab->transform->Rotate(Vector3{ 0.0f,1.0f,0.0f }, frontInform.rotAngle);
			//BuildManager::buildManager->buildings[frontInform] = prefab;
			//prefab->GetComponent<Building>()->positionToAnimate = prefab->transform->position;
			//prefab->transform->position.y -= prefab->GetComponent<BoxCollider>()->extents.y * 2 + 0.5f;

			buildCommand.pop_front();
		}
	}
};

class ButtonManager : public MonoBehavior<ButtonManager>
{
private /*이 영역에 private 변수를 선언하세요.*/:

public  /*이 영역에 public 변수를 선언하세요.*/:
	static ButtonManager* buttonManager;

	std::vector<std::pair<GameObject*, bool>> buttons;

private:
	friend class GameObject;
	friend class MonoBehavior<ButtonManager>;
	ButtonManager() = default;
	ButtonManager(ButtonManager&) = default;

public:
	~ButtonManager() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
	}

	void SelectButton(int buttonIndex)
	{
		buttons[buttonIndex].second ? DisableButton(buttonIndex) : EnableButton(buttonIndex);
	}

	void EnableButton(int buttonIndex)
	{
		buttons[buttonIndex].first->SetActive(true);
		buttons[buttonIndex].second = true;
	}

	void DisableButton(int buttonIndex)
	{
		//gameObject->scene->PushDisable(buttons[buttonIndex].first);
		buttons[buttonIndex].first->SetActive(false);
		buttons[buttonIndex].second = false;
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};