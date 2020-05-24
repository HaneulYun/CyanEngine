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

void AnimatorController::GetFinalTransforms(const AnimatorControllerState* state, float timePos,
	std::vector<PastState>& pastStates, std::vector<Matrix4x4>& finalTransforms) const
{
	UINT numBones = mBoneOffsets.size();
	std::vector<Matrix4x4> toParentTransforms(numBones);

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

		Vector3 DesT, DesS;
		Vector3 SrcT, SrcS;
		Vector4 DesR;
		Vector4 SrcR;

		state->motion->BoneAnimations[i].Interpolate(timePos, T, S, R);
		DesT.xmf3 = T;
		DesS.xmf3 = S;
		DesR.xmf4 = R;
		for (auto& pastState : pastStates)
		{
			if(!i)
				weightTotal += pastState.TransitionWeight;

			pastState.state->motion->BoneAnimations[i].Interpolate(pastState.TimePos, T, S, R);
			lerpPercent = pastState.TransitionWeight / weightTotal;
			SrcT.xmf3 = T;
			SrcS.xmf3 = S;
			SrcR.xmf4 = R;
		
			DesT = Vector3::Lerp(DesT, SrcT, lerpPercent);
			DesS = Vector3::Lerp(DesS, SrcS, lerpPercent);
			DesR = Vector4::QuaternionSlerp(DesR, SrcR, lerpPercent);
		}

		Vector4 zero{ 0, 0, 0, 1 };
		toParentTransforms[i] = Matrix4x4::MatrixAffineTransformation(DesS, zero, DesR, DesT);
	}

	std::vector<Matrix4x4> toRootTransforms(numBones);
	toRootTransforms[0] = toParentTransforms[0];
	for (UINT i = 1; i < numBones; ++i)
	{
		int parentIndex = mBoneHierarchy[i];
		Matrix4x4 parentToRoot = toRootTransforms[parentIndex];
		Matrix4x4 toParent = toParentTransforms[i];
		Matrix4x4 toRoot = toParent * parentToRoot;
		toRootTransforms[i] = toRoot;
	}

	for (UINT i = 0; i < numBones; ++i)
	{
		Matrix4x4 offset = mBoneOffsets[i];
		Matrix4x4 toRoot = toRootTransforms[i];
		Matrix4x4 finalTransform = offset * toRoot;
		finalTransforms[i] = finalTransform.Transpose();
	}
}

AnimatorControllerState* AnimatorController::Transition(AnimatorControllerState* state)
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
				case AnimatorControllerStateTransitionConditionOperatorType::Greater:
					flag += !(parameter.Float > condition.Float);
					break;
				case AnimatorControllerStateTransitionConditionOperatorType::Less:
					flag += !(parameter.Float < condition.Float);
					break;
				}
				break;
			case AnimatorControllerParameterType::Int:
				switch (condition.operatorType)
				{
				case AnimatorControllerStateTransitionConditionOperatorType::Greater:
					flag += !(parameter.Int > condition.Int);
					break;
				case AnimatorControllerStateTransitionConditionOperatorType::Less:
					flag += !(parameter.Int > condition.Int);
					break;
				case AnimatorControllerStateTransitionConditionOperatorType::Equals:
					flag += !(parameter.Int > condition.Int);
					break;
				case AnimatorControllerStateTransitionConditionOperatorType::NotEqual:
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
