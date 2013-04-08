//---------------------------------------------------------------------------------------
// File: BasicModel.h
//
// This class contains a MeshGeometry instance (specifying the geometry of the model).
// It also contains the materials needed, and it contains CPU copies of the mesh data.
//---------------------------------------------------------------------------------------

#ifndef BASICMODEL_H_
#define BASICMODEL_H_

#include <d3dUtilities.h>
#include <TextureManager.h>
#include "ObjLoader.h"
#include "MeshGeometry.h"
#include <xnacollision.h>
#include "GenericObjectLoader.h"

class BasicModel
{
public:
	BasicModel(ID3D11Device* device, TextureManager& textureMgr, const std::string& fileName, const std::wstring& texturePath);
	BasicModel(ID3D11Device* device, TextureManager& textureMgr, const std::string& modelFilename, const std::string& modelFolderPath, const std::wstring& texturePath);
	~BasicModel(void);

	UINT subsetCount;
	UINT materialCount;

	std::vector<Material> mat;
	std::vector<ID3D11ShaderResourceView*> diffuseMapSRV;

	// CPU copies of mesh data 
	std::vector<Vertex::Basic32> vertices;
	std::vector<UINT> indices;
	std::vector<MeshGeometry::Subset> subsets;

	MeshGeometry modelMesh;

	// Bounding box
	XNA::AxisAlignedBox boundingBox;
};

struct BasicModelInstance
{
	BasicModel* model;
	XMFLOAT4X4 world;
	bool isVisible;
};

#endif