#include "SkinnedData.h"

/*
KeyFrame::KeyFrame()
	: timePos(0.0f),
	translation(0.0f, 0.0f, 0.0f),
	scale(1.0f, 1.0f, 1.0f),
	rotationQuat(0.0f, 0.0f, 0.0f, 1.0f)
{
}

KeyFrame::~KeyFrame()
{
}

//=======================================================================================
// Bone animation
//=======================================================================================
float BoneAnimation::getStartTime() const
{
	// Keyframes are sorted by time, so first keyframe equals start time
	return keyFrames.front().timePos;
}

float BoneAnimation::getEndTime() const
{
	// Keyframes are sorted by time, so last keyframe equals end time
	float f = keyFrames.back().timePos;

	return f;
}

void BoneAnimation::interpolate(float time, XMFLOAT4X4& M) const
{
	// First keyframe
	if (time <= keyFrames.front().timePos)
	{
		XMVECTOR scale = XMLoadFloat3(&keyFrames.front().scale);
		XMVECTOR pos = XMLoadFloat3(&keyFrames.front().translation);
		XMVECTOR rotQuat = XMLoadFloat4(&keyFrames.front().rotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(scale, zero, pos, rotQuat));
	}
	// Last keyframe
	else if (time >= keyFrames.back().timePos)
	{
		XMVECTOR scale = XMLoadFloat3(&keyFrames.back().scale);
		XMVECTOR pos = XMLoadFloat3(&keyFrames.back().translation);
		XMVECTOR rotQuat = XMLoadFloat4(&keyFrames.back().rotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(scale, zero, pos, rotQuat));
	}
	// Interpolate between two keyframes
	else
	{
		for (UINT i = 0; i < keyFrames.size()-1; ++i)
		{
			if (time >= keyFrames[i].timePos && time <= keyFrames[i+1].timePos)
			{
				float lerpPercent = (time - keyFrames[i].timePos) / (keyFrames[i+1].timePos - keyFrames[i].timePos);

				XMVECTOR scale0 = XMLoadFloat3(&keyFrames[i].scale);
				XMVECTOR scale1 = XMLoadFloat3(&keyFrames[i+1].scale);

				XMVECTOR pos0 = XMLoadFloat3(&keyFrames[i].translation);
				XMVECTOR pos1 = XMLoadFloat3(&keyFrames[i+1].translation);

				XMVECTOR rotQuat0 = XMLoadFloat4(&keyFrames[i].rotationQuat);
				XMVECTOR rotQuat1 = XMLoadFloat4(&keyFrames[i+1].rotationQuat);

				XMVECTOR scale = XMVectorLerp(scale0, scale1, lerpPercent);
				XMVECTOR pos = XMVectorLerp(pos0, pos1, lerpPercent);
				XMVECTOR rotQuat = XMQuaternionSlerp(rotQuat0, rotQuat1, lerpPercent);

				XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
				XMStoreFloat4x4(&M, XMMatrixAffineTransformation(scale, zero, rotQuat, pos));

				break;
			}
		}
	}
}

//=======================================================================================
// Animation clip
//=======================================================================================
float AnimationClip::getClipStartTime() const
{
	float time = MathHelper::infinity;

	// Iterate through all the bones in this clip and find the smallest start time
	for (UINT i = 0; i < boneAnimations.size(); ++i)
	{
		time = MathHelper::getMin(time, boneAnimations[i].getStartTime());
	}

	return time;
}

float AnimationClip::getClipEndTime() const
{
	float time = 0.0f;

	// Iterate through all the bones in this clip and find the largest end time
	for (UINT i = 0; i < boneAnimations.size(); ++i)
	{
		time = MathHelper::getMax(time, boneAnimations[i].getEndTime());
	}

	return time;
}

void AnimationClip::interpolate(float time, std::vector<XMFLOAT4X4>& boneTransforms) const
{
	for (UINT i = 0; i < boneAnimations.size(); ++i)
	{
		boneAnimations[i].interpolate(time, boneTransforms[i]);
	}
}

//=======================================================================================
// Skinned data
//=======================================================================================
float SkinnedData::getClipStartTime(const std::string& clipName) const
{
	auto clip = mAnimations.find(clipName);

	return clip->second.getClipStartTime();
}

float SkinnedData::getClipEndTime(const std::string& clipName) const
{
	auto clip = mAnimations.find(clipName);

	return clip->second.getClipEndTime();
}

UINT SkinnedData::getBoneCount() const
{
	return mBoneHierarchy.size();
}

void SkinnedData::set(std::vector<int>& boneHierarchy,
					  std::vector<XMFLOAT4X4>& boneOffsets,
					  std::map<std::string, AnimationClip>& animations)
{
	mBoneHierarchy = boneHierarchy;
	mBoneOffsets = boneOffsets;
	mAnimations = animations;
}

void SkinnedData::getFinalTransforms(const std::string& clipName,
									 float timePos, std::vector<XMFLOAT4X4>& finalTransforms) const
{
	UINT numBones = mBoneOffsets.size();

	std::vector<XMFLOAT4X4> toParentTransforms(numBones);

	// Interpolate all bones in this clip at given time instance
	auto clip = mAnimations.find(clipName);
	clip->second.interpolate(timePos, toParentTransforms);

	//----------------------------------------------------------------
	// Traverse hierarchy and transform all the bones to root space
	//----------------------------------------------------------------
	std::vector<XMFLOAT4X4> toRootTransforms(numBones);

	// Root bone has index 0. It has no parent so toRootTransform is
	// just its local bone transform
	toRootTransforms[0] = toParentTransforms[0];

	// Now find toRootTransform for all the other bones
	for (UINT i = 0; i < numBones; ++i)
	{
		XMMATRIX toParent = XMLoadFloat4x4(&toParentTransforms[i]);

		int parentIndex = mBoneHierarchy[i];

		XMMATRIX parentToRoot = XMLoadFloat4x4(&toRootTransforms[parentIndex]);

		XMMATRIX toRoot = XMMatrixMultiply(toParent, parentToRoot);

		XMStoreFloat4x4(&toRootTransforms[i], toRoot);
	}

	// Pre-multiply by the bone offset transform to get the final transform
	for (UINT i = 0; i < numBones; ++i)
	{
		XMMATRIX offset = XMLoadFloat4x4(&mBoneOffsets[i]);
		XMMATRIX toRoot = XMLoadFloat4x4(&toRootTransforms[i]);
		
		XMStoreFloat4x4(&finalTransforms[i], XMMatrixMultiply(offset, toRoot));
	}
}
*/