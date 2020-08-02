#include "pch.h"
#include "Transform.h"

void Transform::OnDestroy()
{
	if (gameObject->scene)
		gameObject->scene->spatialPartitioningManager.DeleteGameObject(gameObject);
}
