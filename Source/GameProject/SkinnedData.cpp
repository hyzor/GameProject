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

AnimEvaluator::AnimEvaluator(const aiAnimation* anim)
{
	mLastTime = 0.0f;

	// Find ticks per seconds, if not specified (0) then set it to 100
	mTicksPerSecond = static_cast<float>(anim->mTicksPerSecond != 0.0f ? anim->mTicksPerSecond : 100.0f);

	mDuration = static_cast<float>(anim->mDuration);
	Name = std::string(anim->mName.data, anim->mName.length);

	// No animation name was found
	if (Name.size() == 0)
	{
		// Set animation name to "Animation + AnimationIndexer"
		std::ostringstream strStream;
		strStream << "Animation" << AnimationIndexer;
		Name = strStream.str();
	}

	// Load all the channels for this animation
	Channels.resize(anim->mNumChannels);
	for (UINT i = 0; i < anim->mNumChannels; ++i)
	{
		Channels[i].Name = anim->mChannels[i]->mNodeName.data;

		// Load Position keys
		for (UINT j = 0; j < anim->mChannels[i]->mNumPositionKeys; ++j)
			Channels[i].PositionKeys.push_back(anim->mChannels[i]->mPositionKeys[j]);

		// Load Rotation keys
		for (UINT j = 0; j < anim->mChannels[i]->mNumRotationKeys; ++j)
			Channels[i].RotationKeys.push_back(anim->mChannels[i]->mRotationKeys[j]);

		// Load Scaling keys
		for (UINT j = 0; j < anim->mChannels[i]->mNumScalingKeys; ++j)
			Channels[i].ScalingKeys.push_back(anim->mChannels[i]->mScalingKeys[j]);
	}

	mLastPositions.resize(anim->mNumChannels, std::make_tuple(0, 0, 0));
}

UINT AnimEvaluator::GetFrameIndexAt(float time)
{
	time *= mTicksPerSecond;

	float _time = 0.0f;
	if (mDuration > 0.0)
		_time = fmod(time, mDuration);

	float percent = _time / mDuration;

	if (!PlayAnimationForward)
		percent = (percent - 1.0f) * -1.0f;

	UINT frameIndex = static_cast<UINT>((static_cast<float>(Transforms.size()) * percent));

	return frameIndex;
}

UINT AnimEvaluator::GetFrameIndexAt( float time, UINT frameStart, UINT frameEnd )
{
	UINT nrOfFrames = (frameEnd - frameStart) + 1;
	float framesDuration = (float)nrOfFrames * (mDuration / static_cast<float>(Transforms.size()));

	time *= mTicksPerSecond;

	float _time = 0.0f;
	if (framesDuration > 0.0)
		_time = fmod(time, framesDuration);

	float percent = _time / framesDuration;

	if (!PlayAnimationForward)
		percent = (percent - 1.0f) * -1.0f;

	UINT frameIndex = static_cast<UINT>((static_cast<float>(nrOfFrames) * percent));

	frameIndex += (frameStart - 1);

	if (frameIndex+1 > frameEnd)
		int test = 0;

	if (frameIndex+1 < frameStart)
		int test = 0;

	return frameIndex;
}

UINT AnimEvaluator::GetFrameIndexAt( float time, UINT frameStart, UINT frameEnd, bool playAnimForward )
{
	UINT nrOfFrames = (frameEnd - frameStart) + 1;
	float framesDuration = (float)nrOfFrames * (mDuration / static_cast<float>(Transforms.size()));

	time *= mTicksPerSecond;

	float _time = 0.0f;
	if (framesDuration > 0.0)
		_time = fmod(time, framesDuration);

	float percent = _time / framesDuration;

	if (!playAnimForward)
		percent = (percent - 1.0f) * -1.0f;

	UINT frameIndex = static_cast<UINT>((static_cast<float>(nrOfFrames) * percent));

	frameIndex += (frameStart - 1);

	return frameIndex;
}

void AnimEvaluator::Evaluate( float pTime, std::map<std::string, SkinData::Bone*>& bones )
{
	pTime *= mTicksPerSecond;

	float time = 0.0f;
	if (mDuration > 0.0)
		time = fmod(pTime, mDuration);

	// Calculate transformations for each channel
	for (UINT i = 0; i < Channels.size(); ++i)
	{
		const SkinData::AnimationChannel* channel = &Channels[i];
		std::map<std::string, SkinData::Bone*>::iterator boneNode = bones.find(channel->Name);

		// Did not find bone node
		if (boneNode == bones.end())
			continue;

		//---------------------------------------------
		// Position
		//---------------------------------------------
		aiVector3D presentPosition(0, 0, 0);
		if (channel->PositionKeys.size() > 0)
		{
			// Look for present frame number
			UINT frame = (time >= mLastTime) ? std::get<0>(mLastPositions[i]) : 0;

			while (frame < channel->PositionKeys.size() - 1)
			{
				if (time < channel->PositionKeys[frame+1].mTime)
					break;

				frame++;
			}

			// Interpolate between this frame's value and the next frame's value
			UINT nextFrame = (frame + 1) % channel->PositionKeys.size();

			const aiVectorKey& key = channel->PositionKeys[frame];
			const aiVectorKey& nextKey = channel->PositionKeys[nextFrame];

			double diffTime = nextKey.mTime - key.mTime;

			if (diffTime < 0.0)
				diffTime += mDuration;
			if (diffTime > 0)
			{
				float factor = float((time - key.mTime) / diffTime);
				presentPosition = key.mValue + (nextKey.mValue - key.mValue) * factor;
			}
			else
			{
				presentPosition = key.mValue;
			}

			std::get<0>(mLastPositions[i]) = frame;
		}

		//---------------------------------------------
		// Rotation
		//---------------------------------------------
		aiQuaternion presentRotation(1, 0, 0, 0);
		if (channel->RotationKeys.size() > 0)
		{
			UINT frame = (time >= mLastTime) ? std::get<1>(mLastPositions[i]) : 0;

			while (frame < channel->RotationKeys.size() - 1)
			{
				if (time < channel->RotationKeys[frame+1].mTime)
					break;

				frame++;
			}

			UINT nextFrame = (frame + 1) % channel->RotationKeys.size();

			const aiQuatKey& key = channel->RotationKeys[frame];
			const aiQuatKey& nextKey = channel->RotationKeys[nextFrame];
			
			double diffTime = nextKey.mTime - key.mTime;

			if (diffTime < 0.0)
				diffTime += mDuration;

			if (diffTime > 0)
			{
				float factor = float((time - key.mTime) / diffTime);
				aiQuaternion::Interpolate(presentRotation, key.mValue, nextKey.mValue, factor);
			}
			else
			{
				presentRotation = key.mValue;
			}

			std::get<1>(mLastPositions[i]) = frame;
		}

		//---------------------------------------------
		// Scaling
		//---------------------------------------------
		aiVector3D presentScaling(1, 1, 1);
		if (channel->ScalingKeys.size() > 0)
		{
			UINT frame = (time >= mLastTime) ? std::get<2>(mLastPositions[i]) : 0;

			while (frame < channel->ScalingKeys.size() - 1)
			{
				if (time < channel->ScalingKeys[frame+1].mTime)
					break;

				frame++;
			}

			presentScaling = channel->ScalingKeys[frame].mValue;
			std::get<2>(mLastPositions[i]) = frame;
		}

		aiMatrix4x4 mat = aiMatrix4x4(presentRotation.GetMatrix());

		mat.a1 *= presentScaling.x; mat.b1 *= presentScaling.x; mat.c1 *= presentScaling.x;
		mat.a2 *= presentScaling.y; mat.b2 *= presentScaling.y; mat.c2 *= presentScaling.y;
		mat.a3 *= presentScaling.z; mat.b3 *= presentScaling.z; mat.c3 *= presentScaling.z;
		mat.a4 = presentPosition.x; mat.b4 = presentPosition.y; mat.c4 = presentPosition.z;
		mat.Transpose();

		SkinData::ReadAiMatrix(boneNode->second->LocalTransform, mat);

	} // Channel end

	mLastTime = time;
}

SkinnedData::~SkinnedData()
{
	CurrentAnimIndex = -1;
	Animations.clear();
	SafeDelete(Skeleton);
}

bool SkinnedData::SetAnimation( const std::string& name )
{
	std::map<std::string, UINT>::iterator itr = AnimationNameToId.find(name);
	UINT oldIndex = CurrentAnimIndex;

	if (itr != AnimationNameToId.end())
		CurrentAnimIndex = itr->second;

	return oldIndex != CurrentAnimIndex;
}

bool SkinnedData::SetAnimIndex(UINT animIndex)
{
	if ((size_t)animIndex >= Animations.size())
		return false;

	UINT oldIndex = CurrentAnimIndex;
	CurrentAnimIndex = animIndex;

	return oldIndex != CurrentAnimIndex;
}

void SkinnedData::Calculate( float time )
{
	if ((CurrentAnimIndex < 0) | (CurrentAnimIndex >= Animations.size()))
		return;

	Animations[CurrentAnimIndex].Evaluate(time, BonesByName);
	UpdateTransforms(Skeleton);
}

void SkinnedData::UpdateTransforms( SkinData::Bone* node )
{
	SkinData::CalculateBoneToWorldTransform(node);

	for (std::vector<SkinData::Bone*>::iterator it = node->Children.begin(); it != node->Children.end(); ++it)
	{
		UpdateTransforms(*it);
	}
}

void SkinnedData::CalcBoneMatrices()
{
	for (UINT i = 0; i < Transforms.size(); ++i)
	{
		XMMATRIX offsetMatrix = XMLoadFloat4x4(&Bones[i]->Offset);
		XMMATRIX globalTransform = XMLoadFloat4x4(&Bones[i]->GlobalTransform);
		XMStoreFloat4x4(&Transforms[i], XMMatrixMultiply(offsetMatrix, globalTransform));
	}
}

UINT SkinnedData::GetAnimationIndex( const std::string& name )
{
	std::map<std::string, UINT>::iterator itr = AnimationNameToId.find(name);
	UINT animIndex = 0;

	if (itr != AnimationNameToId.end())
		animIndex = itr->second;

	return animIndex;
}
