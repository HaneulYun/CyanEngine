#include "pch.h"
#include "QuadTree.h"

void QuadTree::AddGameObject(GameObject* gameObject)
{
	root->AddGameObject(gameObject);
}
