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