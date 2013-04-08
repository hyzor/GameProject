#ifndef GENERICOBJECTLOADER_H_
#define GENERICOBJECTLOADER_H_

#include <d3dUtilities.h>
#include "Vertex.h"
#include "MeshGeometry.h"
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

	// Old loaders
	bool loadObject(const std::string& fileName,
		std::vector<Vertex::Basic32>& vertices,
		std::vector<UINT>& indices,
		std::vector<MeshGeometry::Subset>& subsets,
		std::vector<GenericMaterial>& materials);

	bool loadSkinnedObject(const std::string& fileName,
		std::vector<Vertex::Basic32>& vertices,
		std::vector<UINT>& indices,
		std::vector<MeshGeometry::Subset>& subsets,
		std::vector<GenericMaterial>& materials,
		SkinnedData& skinData);

	// Meshes
// 	bool loadObject(
// 		const std::string& fileName,
// 		std::vector<GenericMaterial>& materials,
// 		std::vector<GenericMesh>& meshes,
// 		ID3D11Device* device);

	bool loadObject(
		const std::string& fileName,
		std::vector<GenericMaterial>& materials,
		std::vector<GenericMesh>& meshes);

	bool loadSkinnedObject(
		const std::string& fileName,
		std::vector<GenericMaterial>& materials,
		std::vector<GenericSkinnedMesh>& meshes,
		SkinnedData& skinData,
		std::vector<SkinnedDataStructs::Bone>& bones);

private:
	void initMaterial(GenericMaterial* mat);
	void loadMaterials(const aiScene* scene, std::vector<GenericMaterial>& materials);

	void readBoneOffsets(aiBone** bones, std::vector<XMFLOAT4X4>& boneOffsets);
	void readBoneKeyframes(aiNodeAnim* nodeAnim, BoneAnimation& boneAnimation);

	void ReadNodeHierarchy(const aiNode* node, std::vector<SkinnedDataStructs::Bone>& bones, int parentBoneIndex);
};

#endif