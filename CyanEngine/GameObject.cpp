#include "pch.h"
#include "GameObject.h"

GameObject::GameObject(bool isUI)
{
	if(isUI)
		transform = new RectTransform();
	else
		transform = new Transform();
	transform->gameObject = this;
	components.push_back(transform);
}

GameObject::GameObject(GameObject* original)
{
	for (GameObject* child : original->children)
		AddChild(new GameObject(child));
	for (Component* component : original->components)
		AddComponent(component);
}

void GameObject::Start()
{
	for (Component* component : components)
		component->Start();
	for (GameObject* child : children)
		child->Start();
}

void GameObject::Update()
{
	if (!active)
		return;
	for (Component* component : components)
		if(component->enabled)
			component->Update();
	for (GameObject* child : children)
		child->Update();

	if (instanceIndex > -1)
	{
		auto objectsResource = renderSet->GetResources();
		auto instanceBuffer = objectsResource->InstanceBuffer.get();
		auto skinnedBuffer = objectsResource->SkinnedBuffer.get();
		auto matIndexBuffer = objectsResource->MatIndexBuffer.get();

		// instance data
		if (NumFramesDirty)
		{
			Matrix4x4 worldTransform;
			if (layer == (int)RenderLayer::UI)
				worldTransform = RectTransform::Transform(GetMatrix());
			else
				worldTransform = GetMatrix();
			Matrix4x4 world = worldTransform;
			Matrix4x4 texTransform = TexTransform;

			InstanceData objConstants;
			objConstants.World = world.Transpose();
			objConstants.TexTransform = texTransform.Transpose();
			objConstants.MaterialIndexStride = 0;
			objConstants.BoneTransformStride = 0;

			Renderer* renderer = GetComponent<Renderer>();
			if (!renderer)
				renderer = GetComponent<SkinnedMeshRenderer>();
			if (renderer)
				objConstants.MaterialIndexStride = renderer->materials.size();
			if (GetComponent<Animator>())
				objConstants.BoneTransformStride = GetComponent<Animator>()->controller->BoneCount();

			instanceBuffer->CopyData(instanceIndex, objConstants);

			if (NumFramesDirty > 0)
				--NumFramesDirty;
		}

		// skinned data
		if (GetComponent<Animator>())
		{
			int baseindex = instanceIndex * GetComponent<Animator>()->controller->BoneCount();

			GetComponent<Animator>()->UpdateSkinnedAnimation(Time::deltaTime);
			for (int i = 0; i < GetComponent<Animator>()->FinalTransforms.size(); ++i)
			{
				SkinnnedData skinnedConstants;
				skinnedConstants.BoneTransforms = GetComponent<Animator>()->FinalTransforms[i];
				skinnedBuffer->CopyData(baseindex + i, skinnedConstants);
			}
		}

		// material data
		Renderer* renderer = GetComponent<Renderer>();
		if (!renderer)
			renderer = GetComponent<SkinnedMeshRenderer>();
		if (renderer)
		{
			int baseindex = instanceIndex * renderer->materials.size();

			for (int i = 0; i < renderer->materials.size(); ++i)
			{
				MatIndexData skinnedConstants;
				skinnedConstants.MaterialIndex = renderer->materials[i]->MatCBIndex;
				matIndexBuffer->CopyData(baseindex + i, skinnedConstants);
			}
		}
	}
}

void GameObject::OnTriggerEnter(GameObject* other)
{
	for (Component* component : components)
		component->OnTriggerEnter(other);
}

void GameObject::OnTriggerStay(GameObject* other)
{
	for (Component* component : components)
		component->OnTriggerStay(other);
}

void GameObject::OnTriggerExit(GameObject* other)
{
	for (Component* component : components)
		component->OnTriggerExit(other);
}

void GameObject::OnCollisionEnter(GameObject* other)
{
	for (Component* component : components)
		component->OnCollisionEnter(other);
}

void GameObject::OnCollisionStay(GameObject* other)
{
	for (Component* component : components)
		component->OnCollisionStay(other);
}

void GameObject::OnCollisionExit(GameObject* other)
{
	for (Component* component : components)
		component->OnCollisionExit(other);
}

Matrix4x4 GameObject::GetMatrix()
{
	if (parent)
		return transform->localToWorldMatrix * parent->GetMatrix();
	return transform->localToWorldMatrix;
}

void GameObject::SetScene(Scene* scene)
{
	this->scene = scene;
	for (GameObject* child : children)
		child->SetScene(scene);
}

void GameObject::SetActive(bool state)
{
	if (active != state)
		if (renderSet)
			renderSet->isDirty = NUM_FRAME_RESOURCES;
	active = state;
	for (GameObject* child : children)
		child->SetActive(state);
}

void GameObject::ReleaseRenderSet()
{
	for (auto& child : children)
		child->ReleaseRenderSet();
	if (renderSet)
	{
		for (auto iter = renderSet->gameObjects.begin(); iter != renderSet->gameObjects.end(); ++iter)
			if (*iter == this)
			{
				renderSet->isDirty = NUM_FRAME_RESOURCES;
				renderSet->gameObjects.erase(iter);
				break;
			}
	}
}
