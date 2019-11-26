#include "pch.h"
#include "BoxCollider.h"

bool BoxCollider::Compare(Collider* _other)
{
	if (typeid(*_other).name() == typeid(BoxCollider).name())
	{
		BoxCollider* other = dynamic_cast<BoxCollider*>(_other);
		boundingBox.Center = gameObject->transform->position.xmf3;
		other->boundingBox.Center = other->gameObject->transform->position.xmf3;

		return boundingBox.Intersects(other->boundingBox);
	}
	else if (typeid(*_other).name() == typeid(SphereCollider).name())
	{
		SphereCollider* other = dynamic_cast<SphereCollider*>(_other);
		boundingBox.Center = gameObject->transform->position.xmf3;
		other->boundingSphere.Center = other->gameObject->transform->position.xmf3;
		return boundingBox.Intersects(other->boundingSphere);
	}
}