#include "pch.h"
#include "Transform.h"

void Transform::OnDestroy()
{
	gameObject->scene->spatialPartitioningManager.DeleteGameObject(gameObject);
}
