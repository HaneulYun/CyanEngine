#include "pch.h"
#include "BoxCollider.h"

void BoxCollider::Start()
{
	gameObject->scene->spatialPartitioningManager.AddGameObject(gameObject);
}

bool BoxCollider::Compare(Collider* _other)
{
	auto mtx = gameObject->GetMatrix();
	BoundingBox bx{ (center.TransformCoord(mtx)).xmf3, extents.xmf3 };

	BoundingOrientedBox obbBox{};
	if (obb)
	{
		obbBox.Center = bx.Center;
		obbBox.Extents = bx.Extents;
		obbBox.Orientation = gameObject->transform->localToWorldMatrix.QuaternionRotationMatrix().xmf4;
	}
	if (typeid(*_other).name() == typeid(BoxCollider).name())
	{
		BoxCollider* other = dynamic_cast<BoxCollider*>(_other);
		auto omtx = other->gameObject->GetMatrix();
		BoundingBox obx{ (other->center.TransformCoord(omtx)).xmf3, other->extents.xmf3 };

		BoundingOrientedBox obbOtherBox{};
		if (other->obb)
		{
			obbOtherBox.Center = obx.Center;
			obbOtherBox.Extents = obx.Extents;
			obbOtherBox.Orientation = other->gameObject->transform->localToWorldMatrix.QuaternionRotationMatrix().xmf4;
		}

		if (obb)
		{
			if (other->obb)
				return obbBox.Intersects(obbOtherBox);
			return obbBox.Intersects(obx);
		}
		if (other->obb)
			return bx.Intersects(obbOtherBox);
		return bx.Intersects(obx);
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