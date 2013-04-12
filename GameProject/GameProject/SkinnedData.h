#ifndef SKINNEDDATA_H_
#define SKINNEDDATA_H_

#include <d3dUtilities.h>
#include <map>
#include <assimp/scene.h>
#include <tuple>

namespace SkinData
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
			Name = "";
			Parent = NULL;
		}

		~Bone()
		{
			for (UINT i = 0; i < Children.size(); ++i)
			{
				delete Children[i];
			}
		}

		std::string Name;
		XMFLOAT4X4 Offset;
		XMFLOAT4X4 LocalTransform;
		XMFLOAT4X4 GlobalTransform;
		XMFLOAT4X4 OriginalLocalTransform;
		XMFLOAT4X4 FinalTransformation;
		std::vector<VertexWeight> Weights;

		Bone* Parent;
		std::vector<Bone*> Children;
	};

	struct AnimationChannel
	{
		std::string Name;
		std::vector<aiVectorKey> PositionKeys;
		std::vector<aiQuatKey> RotationKeys;
		std::vector<aiVectorKey> ScalingKeys;
	};

	static void ReadAiMatrix(XMFLOAT4X4& out, const aiMatrix4x4& in)
	{
		out._11=in.a1;
		out._12=in.a2;
		out._13=in.a3;
		out._14=in.a4;

		out._21=in.b1;
		out._22=in.b2;
		out._23=in.b3;
		out._24=in.b4;

		out._31=in.c1;
		out._32=in.c2;
		out._33=in.c3;
		out._34=in.c4;

		out._41=in.d1;
		out._42=in.d2;
		out._43=in.d3;
		out._44=in.d4;
	}

	static void CalculateBoneToWorldTransform(SkinData::Bone* child)
	{
		child->GlobalTransform = child->LocalTransform;
		XMMATRIX Child_GlobalTransform = XMLoadFloat4x4(&child->GlobalTransform);

		SkinData::Bone* parent = child->Parent;

		// Climb up towards the root bone
		while (parent)
		{
			XMMATRIX Parent_LocalTransform = XMLoadFloat4x4(&parent->LocalTransform);

			XMMatrixMultiply(Child_GlobalTransform, Parent_LocalTransform);

			//child->GlobalTransform *= parent->LocalTransform;
			parent = parent->Parent;
		}

		// The final to world transform has been calculated, store it in child's GlobalTransform
		XMStoreFloat4x4(&child->GlobalTransform, Child_GlobalTransform);
	}
}

class AnimEvaluator
{
public:
	AnimEvaluator()
	{
		mLastTime = 0.0f;
		mTicksPerSecond = 0.0f;
		mDuration = 0.0f;
		AnimationIndexer = 0;
		PlayAnimationForward = true;
	}

	AnimEvaluator(const aiAnimation* anim);
	void Evaluate(float time, std::map<std::string, SkinData::Bone*>& bones);
	std::vector<XMFLOAT4X4>& GetTransforms(float dt) { return Transforms[GetFrameIndexAt(dt)]; }
	UINT GetFrameIndexAt(float time);

	std::string Name;
	// If the animation has no name, it's name will be
	// Animation + AnimationIndexer
	UINT AnimationIndexer;
	std::vector<SkinData::AnimationChannel> Channels;
	float mLastTime, mTicksPerSecond, mDuration;
	bool PlayAnimationForward;
	std::vector<std::tuple<UINT, UINT, UINT>> mLastPositions;
	std::vector<std::vector<XMFLOAT4X4>> Transforms;
};

class SkinnedData
{
public:
	SkinnedData();
	~SkinnedData();

	bool SetAnimIndex(UINT animIndex);
	bool SetAnimation(const std::string& name);

	void CalcBoneMatrices();
	void Calculate(float time);
	void UpdateTransforms(SkinData::Bone* node);

	SkinData::Bone* Skeleton; // Root node of scene
	std::map<std::string, SkinData::Bone*> BonesByName;
	std::map<std::string, UINT> BonesToIndex;
	std::map<std::string, UINT> AnimationNameToId;
	std::vector<SkinData::Bone*> Bones;
	std::vector<XMFLOAT4X4> Transforms;

	std::vector<AnimEvaluator> Animations;
	UINT CurrentAnimIndex;
};

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