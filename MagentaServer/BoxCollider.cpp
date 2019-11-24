#include "pch.h"
#include "BoxCollider.h"

bool BoxCollider::Compare(BoxCollider* other)
{
	boundingBox.Center = gameObject->transform->position.xmf3;
	other->boundingBox.Center = other->gameObject->transform->position.xmf3;
	return boundingBox.Intersects(other->boundingBox);
}