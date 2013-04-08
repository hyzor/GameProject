//---------------------------------------------------------------------------------------
// File: BasicModel.h
//
// This class contains a MeshGeometry instance (specifying the geometry of the model).
// It also contains the materials needed, and it contains CPU copies of the mesh data.
//---------------------------------------------------------------------------------------

#ifndef GENERICSKINNEDMODEL_H_
#define GENERICSKINNEDMODEL_H_

#include <d3dUtilities.h>
#include <TextureManager.h>
#include "ObjLoader.h"
#include "MeshGeometry.h"
#include <xnacollision.h>
#include "GenericObjectLoader.h"
#include "SkinnedData.h"

class GenericSkinnedModel
{
public:
	GenericSkinnedModel(ID3D11Device* device, TextureManager& textureMgr, const std::string& fileName, const std::wstring& texturePath);
	~GenericSkinnedModel(void);

	UINT numMeshes;
	UINT numMaterials;
	UINT numBones;

	std::vector<Material> mat;
	std::vector<ID3D11ShaderResourceView*> diffuseMapSRV;
	std::vector<ID3D11ShaderResourceView*> normalMapSRV;

	std::vector<GenericSkinnedMesh> meshes;

	// Pointer to all vertices of all meshes
	//std::vector<Vertex::PosNormalTexTanSkinned*> vertices;

	//std::vector<Vertex::PosNormalTexTanSkinned> vertices;

	// Map containing vectors of vertices
	// The maps key values are the meshes corresponding index
	std::map<int, std::vector<Vertex::PosNormalTexTanSkinned>> verticesMap;

	std::vector<SkinnedDataStructs::Bone> bones;

	// Bounding box
	XNA::AxisAlignedBox boundingBox;

	SkinnedData skinnedData;
};

struct GenericSkinnedModelInstance
{
	GenericSkinnedModel* model;
	XMFLOAT4X4 world;
	bool isVisible;
	std::vector<XMFLOAT4X4> FinalTransforms;
	std::string ClipName;
	float TimePos;

	void Update(float dt);
};

#endif