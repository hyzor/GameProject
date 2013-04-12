#ifndef GENERICOBJECTLOADER_H_
#define GENERICOBJECTLOADER_H_

#include <d3dUtilities.h>
#include "Vertex.h"
#include "GenericMesh.h"
#include "GenericSkinnedMesh.h"
#include "GenericSkinnedModel.h"

#include "SkinnedData.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Logger.hpp>
#include <assimp/DefaultLogger.hpp>

struct GenericMaterial
{
	Material mat;
	std::wstring diffuseMapName;
	std::wstring normalMapName;
	std::string name;
};

class GenericObjectLoader
{
public:
	GenericObjectLoader(void);
	~GenericObjectLoader(void);

	bool loadObject(
		const std::string& fileName,
		std::vector<GenericMaterial>& materials,
		std::vector<GenericMesh>& meshes);

	bool loadSkinnedObject(
		const std::string& fileName,
		std::vector<GenericMaterial>& materials,
		std::vector<GenericSkinnedMesh>& meshes);

	bool loadSkinnedObject(
		const std::string& fileName,
		std::vector<GenericMaterial>& materials,
		std::vector<GenericSkinnedMesh>& meshes,
		SkinnedData& skinnedData);

private:
	void InitMaterial(GenericMaterial* mat);
	void ReadMaterials(const aiScene* scene, std::vector<GenericMaterial>& materials);

	void ReadBones(aiMesh* mesh, GenericSkinnedMesh& myMesh);
	//void ReadBoneKeyframes(aiNodeAnim* nodeAnim, BoneAnimation& boneAnimation);
	SkinData::Bone* CreateBoneTree(aiNode* node, SkinData::Bone* parent, SkinnedData& skinnedData);
	//void CalculateBoneToWorldTransform(SkinData::Bone* child);
	void ReadAnimations(const aiScene* scene, SkinnedData& skinnedData);

	void SetVertexWeights(GenericSkinnedMesh& myMesh);
};

#endif