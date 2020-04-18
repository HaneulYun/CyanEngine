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
	const AnimationControllerState* nextState, float nextTimePos, float lerpPercent,
	std::vector<XMFLOAT4X4>& finalTransforms)const
{
	UINT numBones = mBoneOffsets.size();
	std::vector<XMFLOAT4X4> toParentTransforms(numBones);

	auto clip0 = state->motion;

	for (UINT i = 0; i < numBones; ++i)
	{
		XMFLOAT3 T0, S0, T1, S1;
		XMFLOAT4 R0, R1;
		XMVECTOR t0, s0, r0, t1, s1, r1;
		XMVECTOR T, S, Q;

		clip0->BoneAnimations[i].Interpolate(timePos, T0, S0, R0);
		t0 = XMLoadFloat3(&T0);
		s0 = XMLoadFloat3(&S0);
		r0 = XMLoadFloat4(&R0);

		if (nextState)
		{
			nextState->motion->BoneAnimations[i].Interpolate(nextTimePos, T1, S1, R1);
			t1 = XMLoadFloat3(&T1);
			s1 = XMLoadFloat3(&S1);
			r1 = XMLoadFloat4(&R1);

			T = XMVectorLerp(t0, t1, lerpPercent);
			S = XMVectorLerp(s0, s1, lerpPercent);
			Q = XMQuaternionSlerp(r0, r1, lerpPercent);
		}
		else
		{
			T = XMLoadFloat3(&T0);
			S = XMLoadFloat3(&S0);
			Q = XMLoadFloat4(&R0);
		}
		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&toParentTransforms[i], XMMatrixAffineTransformation(S, zero, Q, T));
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
