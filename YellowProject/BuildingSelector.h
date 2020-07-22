#pragma once
#include "..\CyanEngine\framework.h"

class BuildingSelector : public MonoBehavior<BuildingSelector>
{
private /*이 영역에 private 변수를 선언하세요.*/:
	int maxPage{ 0 };
	int page{ 0 };

public  /*이 영역에 public 변수를 선언하세요.*/:
	BuildingBuilder::BuildingType type{ BuildingBuilder::BuildingType::None };
	BuildingBuilder* buildingBuilder{ nullptr };

	GameObject* buildingButtons[10]{};

protected:
	friend class GameObject;
	friend class MonoBehavior<BuildingSelector>;
	BuildingSelector() = default;
	BuildingSelector(BuildingSelector&) = default;

public:
	~BuildingSelector() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
		maxPage = buildingBuilder->getBuildingCount(type) / 10;
		void(*events[10])(void*)
		{
			[](void* ptr) { reinterpret_cast<BuildingSelector*>(ptr)->invokeBuilder(0); },
			[](void* ptr) { reinterpret_cast<BuildingSelector*>(ptr)->invokeBuilder(1); },
			[](void* ptr) { reinterpret_cast<BuildingSelector*>(ptr)->invokeBuilder(2); },
			[](void* ptr) { reinterpret_cast<BuildingSelector*>(ptr)->invokeBuilder(3); },
			[](void* ptr) { reinterpret_cast<BuildingSelector*>(ptr)->invokeBuilder(4); },
			[](void* ptr) { reinterpret_cast<BuildingSelector*>(ptr)->invokeBuilder(5); },
			[](void* ptr) { reinterpret_cast<BuildingSelector*>(ptr)->invokeBuilder(6); },
			[](void* ptr) { reinterpret_cast<BuildingSelector*>(ptr)->invokeBuilder(7); },
			[](void* ptr) { reinterpret_cast<BuildingSelector*>(ptr)->invokeBuilder(8); },
			[](void* ptr) { reinterpret_cast<BuildingSelector*>(ptr)->invokeBuilder(9); }
		};

		for (int i = 0; i < 10; ++i)
		{
			auto buildingButton = gameObject->AddChildUI(Scene::scene->CreateImagePrefab());
			addChildAsTextButton(L"", 0.1 * i + 0.05, 0.5, 40, 40, buildingButton)->AddEvent(events[i], this);
			buildingButtons[i] = buildingButton;
		}
		addChildAsTextButton(L"◀", -0.025, 0.5, 20, 20)->AddEvent([](void* ptr) { reinterpret_cast<BuildingSelector*>(ptr)->prevPage(); }, this);
		addChildAsTextButton(L"▶", 1.025, 0.5, 20, 20)->AddEvent([](void* ptr) { reinterpret_cast<BuildingSelector*>(ptr)->nextPage(); }, this);

		gameObject->SetActive(false);
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
	void invokeBuilder(int index)
	{
		buildingBuilder->enterBuildMode(type, page * 10 + index);
	}

	void prevPage()
	{
		if (page > 0)
		{
			--page;
			setBuildingButtonName();
		}
	}

	void nextPage()
	{
		if (page < maxPage)
		{
			++page;
			setBuildingButtonName();
		}
	}

	void setBuildingButtonName()
	{
		for (int i = 0; i < 10; ++i)
		{
			std::wstring text = buildingBuilder->getBuildingName(type, page * 10 + i);
			buildingButtons[i]->GetComponent<Text>()->text = text;
		}
	}

	Button* addChildAsTextButton(std::wstring str, float u, float v, float w, float h, GameObject* _child = nullptr)
	{
		auto child = _child ? _child : gameObject->AddChildUI(Scene::scene->CreateImagePrefab());
		auto rt = child->GetComponent<RectTransform>();
		rt->setAnchorAndPivot(u, v);
		rt->pivot = { 0.5, 0.5 };
		rt->setPosAndSize(0, 0, w, h);

		auto text = child->AddComponent<Text>();
		text->text = str;
		text->fontSize = 10;
		text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
		text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
		Scene::scene->textObjects.push_back(child);

		return child->AddComponent<Button>();
	}
};