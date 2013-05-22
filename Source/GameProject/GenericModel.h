//---------------------------------------------------------------------------------------
// File: BasicModel.h
//
// This class contains a MeshGeometry instance (specifying the geometry of the model).
// It also contains the materials needed, and it contains CPU copies of the mesh data.
//---------------------------------------------------------------------------------------

#ifndef GENERICMODEL_H_
#define GENERICMODEL_H_

#include <d3dUtilities.h>
#include <TextureManager.h>
#include <xnacollision.h>
#include "GenericObjectLoader.h"

class GenericModel
{
public:
	GenericModel(ID3D11Device* device, TextureManager* textureMgr, const std::string& fileName, const std::wstring& texturePath);
	~GenericModel(void);

	UINT meshCount;
	UINT materialCount;

	std::vector<Material> mat;
	std::vector<ID3D11ShaderResourceView*> diffuseMapSRV;
	std::vector<ID3D11ShaderResourceView*> normalMapSRV;

	std::vector<GenericMesh> meshes;

	// Pointer to all vertices of all meshes
	//std::vector<Vertex::Basic32*> vertices;

	const std::vector<XMFLOAT3>& GetVertexPositions() const;
	const XNA::AxisAlignedBox& GetBoundingBox() const;
	const XMFLOAT3& GetVertexMin() const;
	const XMFLOAT3& GetVertexMax() const;

	Vertex::PosNormalTexTan* GetVertex(int index);
	int GetVertexCount();

	void CreatePositions(const std::string& fileName);

private:
	// Vertex vector containing all the positions
	std::vector<XMFLOAT3> mVertexPositions;

	// Bounding box
	XNA::AxisAlignedBox boundingBox;

	XMFLOAT3 mVertexMin, mVertexMax;
};

struct GenericModelInstance
{
	GenericModel* model;
	XMFLOAT4X4 world;
	bool isVisible;
};

#endif