#include "pch.h"
#include "SphereCollider.h"

bool SphereCollider::Compare(Collider* _other)
{
	if (typeid(*_other).name() == typeid(BoxCollider).name())
	{
		BoxCollider* other = dynamic_cast<BoxCollider*>(_other);
		boundingSphere.Center = gameObject->transform->position.xmf3;
		other->boundingBox.Center = other->gameObject->transform->position.xmf3;
		return boundingSphere.Intersects(other->boundingBox);
	}
	else if (typeid(*_other).name() == typeid(SphereCollider).name())
	{
		SphereCollider* other = dynamic_cast<SphereCollider*>(_other);
		boundingSphere.Center = gameObject->transform->position.xmf3;
		other->boundingSphere.Center = other->gameObject->transform->position.xmf3;
		return boundingSphere.Intersects(other->boundingSphere);
	}
}