#ifndef SKINNEDDATA_H_
#define SKINNEDDATA_H_

#include <d3dUtilities.h>
#include <map>

namespace SkinnedDataStructs
{
	struct VertexWeight
	{
		VertexWeight()
		{
			VertexID = 0;
			Weight = 0.0f;
		}

	UINT VertexID;
	float Weight;
	};

	struct Bone
	{
		Bone() 
		{
			name = "";
			//offsetMatrix = NULL;
			//finalTransformation = NULL;

			parentBone = NULL;
		}

		std::string name;
		XMFLOAT4X4 offsetMatrix;
		XMFLOAT4X4 finalTransformation;
		std::vector<VertexWeight> weights;

		Bone* parentBone;
		std::vector<Bone*> childrenBones;
	};
}

/*
struct KeyFrame
{
	KeyFrame();
	~KeyFrame();

	float timePos;
	XMFLOAT3 translation;
	XMFLOAT3 scale;
	XMFLOAT4 rotationQuat;
};

// Contains a list of keyframes, and interpolates
// between them
struct BoneAnimation
{
	float getStartTime() const;
	float getEndTime() const;

	void interpolate(float time, XMFLOAT4X4& M) const;

	std::vector<KeyFrame> keyFrames;
};

// AnimationClips are states like walking, running etc.
// Contains a list of bone animations
struct AnimationClip
{
	float getClipStartTime() const;
	float getClipEndTime() const;

	void interpolate(float time, std::vector<XMFLOAT4X4>& boneTransforms) const;

	std::vector<BoneAnimation> boneAnimations;
};

class SkinnedData
{
public:
	UINT getBoneCount() const;

	float getClipStartTime(const std::string& clipName) const;
	float getClipEndTime(const std::string& clipName) const;

	void set(
		std::vector<int>& boneHierarchy,
		std::vector<XMFLOAT4X4>& boneOffsets,
		std::map<std::string, AnimationClip>& animations);

	void getFinalTransforms(const std::string& clipName, float timePos,
		std::vector<XMFLOAT4X4>& finalTransforms) const;

private:
	std::vector<int> mBoneHierarchy;

	std::vector<XMFLOAT4X4> mBoneOffsets;

	std::map<std::string, AnimationClip> mAnimations;
};
*/

#endif