#include "pch.h"
#include "SphereCollider.h"

bool SphereCollider::Compare(Collider* _other)
{
	boundingSphere.Center = gameObject->transform->position.xmf3;
	if (typeid(*_other).name() == typeid(BoxCollider).name())
	{
		BoxCollider* other = dynamic_cast<BoxCollider*>(_other);
		other->boundingBox.Center = other->gameObject->transform->position.xmf3;

		BoundingOrientedBox obbOtherBox{};
		if (other->obb)
		{
			obbOtherBox.Center = other->boundingBox.Center;
			obbOtherBox.Extents = other->boundingBox.Extents;
			XMStoreFloat4(&obbOtherBox.Orientation, XMQuaternionRotationMatrix(XMLoadFloat4x4(&other->gameObject->transform->localToWorldMatrix)));
		}

		if (other->obb)
			return boundingSphere.Intersects(obbOtherBox);
		return boundingSphere.Intersects(other->boundingBox);
	}
	else if (typeid(*_other).name() == typeid(SphereCollider).name())
	{
		SphereCollider* other = dynamic_cast<SphereCollider*>(_other);
		other->boundingSphere.Center = other->gameObject->transform->position.xmf3;

		return boundingSphere.Intersects(other->boundingSphere);
	}
}