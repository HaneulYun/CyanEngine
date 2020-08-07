#include "pch.h"
#include "Button.h"

void Button::Start()
{
	Scene::scene->buttonObjects.push_back(gameObject);
}

void Button::Update()
{
}

void Button::OnDestroy()
{
	auto iter = find(Scene::scene->buttonObjects.begin(), Scene::scene->buttonObjects.end(), gameObject);

	if (iter != Scene::scene->buttonObjects.end())
		Scene::scene->buttonObjects.erase(iter);
}

bool Button::OnClick()
{
	if (gameObject->active && Input::GetMouseButtonUp(0))
		if (IsPointInRect(Camera::main->ScreenToViewportPoint(Input::mousePosition)))
		{
			for (auto& e : events)
				e.first(e.second);
			Input::mouseUp[0] = false;
			return true;
		}
	return false;
}

bool Button::IsPointInRect(Vector3 position)
{
	RectTransform* rect{ nullptr };
	if (rect = gameObject->GetComponent<RectTransform>(); !rect)
		return false;

	Matrix4x4 mat = RectTransform::Transform(gameObject->GetMatrix());;

	Vector3 leftTop{ mat._41, mat._22 + mat._42, 0 };
	Vector3 rightBottom{ mat._11 + mat._41, mat._42, 0 };

	if (position.x < leftTop.x || position.x > rightBottom.x ||
		position.y < rightBottom.y || position.y > leftTop.y)
		return false;
	return true;
}

void Button::AddEvent(void(*func)(void*), void* data)
{
	events.emplace_back(std::make_pair(func, data));
}