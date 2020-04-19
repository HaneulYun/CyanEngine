#include "pch.h"
#include "AnimatorController.h"

void BoneAnimation::Interpolate(float t, XMFLOAT3& T, XMFLOAT3& S, XMFLOAT4& R) const
{
	if (t <= Keyframes.front().TimePos)
	{
		T = Keyframes.front().Translation;
		S = Keyframes.front().Scale;
		R = Keyframes.front().RotationQuat;
	}
	else if (t >= Keyframes.back().TimePos)
	{
		T = Keyframes.back().Translation;
		S = Keyframes.back().Scale;
		R = Keyframes.back().RotationQuat;
	}
	else
	{
		for (UINT i = 0; i < Keyframes.size() - 1; ++i)
		{
			if (t >= Keyframes[i].TimePos && t <= Keyframes[i + 1].TimePos)
			{
				float lerpPercent = (t - Keyframes[i].TimePos) / (Keyframes[i + 1].TimePos - Keyframes[i].TimePos);

				XMVECTOR t0 = XMLoadFloat3(&Keyframes[i].Translation);
				XMVECTOR t1 = XMLoadFloat3(&Keyframes[i + 1].Translation);

				XMVECTOR s0 = XMLoadFloat3(&Keyframes[i].Scale);
				XMVECTOR s1 = XMLoadFloat3(&Keyframes[i + 1].Scale);

				XMVECTOR r0 = XMLoadFloat4(&Keyframes[i].RotationQuat);
				XMVECTOR r1 = XMLoadFloat4(&Keyframes[i + 1].RotationQuat);

				XMStoreFloat3(&T, XMVectorLerp(t0, t1, lerpPercent));
				XMStoreFloat3(&S, XMVectorLerp(s0, s1, lerpPercent));
				XMStoreFloat4(&R, XMQuaternionSlerp(r0, r1, lerpPercent));
				break;
			}
		}
	}
}

float AnimationClip::GetClipStartTime() const
{
	float t = MathHelper::Infinity;
	for (UINT i = 0; i < BoneAnimations.size(); ++i)
		t = MathHelper::Min(t, BoneAnimations[i].GetStartTime());
	return t;
}

float AnimationClip::GetClipEndTime() const
{
	float t = 0.0f;
	for (UINT i = 0; i < BoneAnimations.size(); ++i)
		t = MathHelper::Max(t, BoneAnimations[i].GetEndTime());
	return t;
}

void AnimatorController::GetFinalTransforms(const AnimationControllerState* state, float timePos,
	std::vector<PastState>& pastStates, std::vector<XMFLOAT4X4>& finalTransforms) const
{
	UINT numBones = mBoneOffsets.size();
	std::vector<XMFLOAT4X4> toParentTransforms(numBones);

	auto clip = state->motion;

	float lerpPercent = 0.0f;
	float weightTotal = 0.0f;
	float weight = 1.0f;
	for (auto& pastState : pastStates)
		weight -= pastState.TransitionWeight;
	weightTotal += weight;

	for (UINT i = 0; i < numBones; ++i)
	{
		XMFLOAT3 T, S;
		XMFLOAT4 R;

		XMVECTOR DesT, DesS, DesR;
		XMVECTOR SrcT, SrcS, SrcR;

		state->motion->BoneAnimations[i].Interpolate(timePos, T, S, R);
		DesT = XMLoadFloat3(&T);
		DesS = XMLoadFloat3(&S);
		DesR = XMLoadFloat4(&R);
		for (auto& pastState : pastStates)
		{
			if(!i)
				weightTotal += pastState.TransitionWeight;

			pastState.state->motion->BoneAnimations[i].Interpolate(pastState.TimePos, T, S, R);
			lerpPercent = pastState.TransitionWeight / weightTotal;
			SrcT = XMLoadFloat3(&T);
			SrcS = XMLoadFloat3(&S);
			SrcR = XMLoadFloat4(&R);
		
			DesT = XMVectorLerp(DesT, SrcT, lerpPercent);
			DesS = XMVectorLerp(DesS, SrcS, lerpPercent);
			DesR = XMQuaternionSlerp(DesR, SrcR, lerpPercent);
		}

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&toParentTransforms[i], XMMatrixAffineTransformation(DesS, zero, DesR, DesT));
	}

	std::vector<XMFLOAT4X4> toRootTransforms(numBones);
	toRootTransforms[0] = toParentTransforms[0];
	for (UINT i = 1; i < numBones; ++i)
	{
		int parentIndex = mBoneHierarchy[i];
		XMMATRIX parentToRoot = XMLoadFloat4x4(&toRootTransforms[parentIndex]);
		XMMATRIX toParent = XMLoadFloat4x4(&toParentTransforms[i]);
		XMMATRIX toRoot = XMMatrixMultiply(toParent, parentToRoot);
		XMStoreFloat4x4(&toRootTransforms[i], toRoot);
	}

	for (UINT i = 0; i < numBones; ++i)
	{
		XMMATRIX offset = XMLoadFloat4x4(&mBoneOffsets[i]);
		XMMATRIX toRoot = XMLoadFloat4x4(&toRootTransforms[i]);
		XMMATRIX finalTransform = XMMatrixMultiply(offset, toRoot);
		XMStoreFloat4x4(&finalTransforms[i], XMMatrixTranspose(finalTransform));
	}
}

AnimationControllerState* AnimatorController::Transition(AnimationControllerState* state)
{
	for (auto& transition : state->transitionns)
	{
		bool flag = false;
		for (auto& condition : transition.conditions)
		{
			auto& parameter = parameters[condition.ParameterName];
			switch (parameter.Type)
			{
			case AnimatorControllerParameterType::Float:
				switch (condition.operatorType)
				{
				case AnimationControllerStateTransitionConditionOperatorType::Greater:
					flag += !(parameter.Float > condition.Float);
					break;
				case AnimationControllerStateTransitionConditionOperatorType::Less:
					flag += !(parameter.Float < condition.Float);
					break;
				}
				break;
			case AnimatorControllerParameterType::Int:
				switch (condition.operatorType)
				{
				case AnimationControllerStateTransitionConditionOperatorType::Greater:
					flag += !(parameter.Int > condition.Int);
					break;
				case AnimationControllerStateTransitionConditionOperatorType::Less:
					flag += !(parameter.Int > condition.Int);
					break;
				case AnimationControllerStateTransitionConditionOperatorType::Equals:
					flag += !(parameter.Int > condition.Int);
					break;
				case AnimationControllerStateTransitionConditionOperatorType::NotEqual:
					flag += !(parameter.Int > condition.Int);
					break;
				}
				break;
			case AnimatorControllerParameterType::Bool:
				flag += !(parameter.Bool == condition.Bool);
				break;
			}
		}
		if (!flag)
		{
			return &states[transition.DestinationStateName];
		}
	}
	return nullptr;
}
