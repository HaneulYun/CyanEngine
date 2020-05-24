#include "pch.h"
#include "BoxCollider.h"

bool BoxCollider::Compare(Collider* _other)
{
	boundingBox.Center = gameObject->transform->position.xmf3;
	BoundingOrientedBox obbBox{};
	if (obb)
	{
		obbBox.Center = boundingBox.Center;
		obbBox.Extents = boundingBox.Extents;
		obbBox.Orientation = gameObject->transform->localToWorldMatrix.QuaternionRotationMatrix().xmf4;
	}
	if (typeid(*_other).name() == typeid(BoxCollider).name())
	{
		BoxCollider* other = dynamic_cast<BoxCollider*>(_other);
		other->boundingBox.Center = other->gameObject->transform->position.xmf3;

		BoundingOrientedBox obbOtherBox{};
		if (other->obb)
		{
			obbOtherBox.Center = other->boundingBox.Center;
			obbOtherBox.Extents = other->boundingBox.Extents;
			obbOtherBox.Orientation = other->gameObject->transform->localToWorldMatrix.QuaternionRotationMatrix().xmf4;
		}

		if (obb)
		{
			if(other->obb)
				return obbBox.Intersects(obbOtherBox);
			return obbBox.Intersects(other->boundingBox);
		}
		if (other->obb)
			return boundingBox.Intersects(obbOtherBox);
		return boundingBox.Intersects(other->boundingBox);
	}
	else if (typeid(*_other).name() == typeid(SphereCollider).name())
	{
		SphereCollider* other = dynamic_cast<SphereCollider*>(_other);
		other->boundingSphere.Center = other->gameObject->transform->position.xmf3;

		if (obb)
			obbBox.Intersects(other->boundingSphere);
		return boundingBox.Intersects(other->boundingSphere);
	}
}