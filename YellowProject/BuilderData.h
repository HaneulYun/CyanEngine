#pragma once

struct BuildingBuilderData
{
	std::wstring buildingName{ L"none" };

	GameObject* prefab{ nullptr };
	Mesh* mesh{ nullptr };
	Material* material{ nullptr };
	std::vector<Material*> materials;
};
