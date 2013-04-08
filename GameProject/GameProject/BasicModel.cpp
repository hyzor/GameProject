//---------------------------------------------------------------------------------------
// File: BasicModel.cpp
//
// This class contains a MeshGeometry instance (specifying the geometry of the model).
// It also contains the materials needed, and it contains CPU copies of the mesh data.
//---------------------------------------------------------------------------------------

#include "BasicModel.h"

//==================================================================================
// Basic model (Any format)
//==================================================================================
BasicModel::BasicModel(ID3D11Device* device, TextureManager& textureMgr, const std::string& fileName, const std::wstring& texturePath)
{
	std::vector<GenericMaterial> mats;

	GenericObjectLoader objectLoader;
	objectLoader.loadObject(fileName, vertices, indices, subsets, mats);

	modelMesh.setVertices(device, &vertices[0], vertices.size());
	modelMesh.setIndices(device, &indices[0], indices.size());
	modelMesh.setSubsetTable(subsets);

	subsetCount = subsets.size();
	materialCount = mats.size();

	for (UINT i = 0; i < materialCount; ++i)
	{
		mat.push_back(mats[i].mat);

		if (mats[i].diffuseMapName != L"")
		{
			ID3D11ShaderResourceView* _diffuseMapSRV = textureMgr.createTexture(texturePath + mats[i].diffuseMapName);
			diffuseMapSRV.push_back(_diffuseMapSRV);
		}
		else
		{
			diffuseMapSRV.push_back(NULL);
		}
	}

	ComputeBoundingAxisAlignedBoxFromPoints(&boundingBox, vertices.size(), &vertices[0].position, sizeof(Vertex::Basic32));

}

//==================================================================================
// Basic model (OBJ format)
//==================================================================================
BasicModel::BasicModel(ID3D11Device* device, TextureManager& textureMgr, const std::string& modelFilename,
	const std::string& modelFolderPath, const std::wstring& texturePath)
{
	std::vector<ObjMaterial> mats;

	// Load mesh
	ObjLoader objLoader;
	objLoader.loadObj(modelFilename, modelFolderPath, vertices, indices, subsets, mats);

	// Set mesh data
	modelMesh.setVertices(device, &vertices[0], vertices.size());
	modelMesh.setIndices(device, &indices[0], indices.size());

	subsetCount = mats.size();

	for (UINT i = 0; i < subsetCount; ++i)
	{
		subsets[i].MaterialIndex = i;
	}

	modelMesh.setSubsetTable(subsets);


	for (UINT i = 0; i < subsetCount; ++i)
	{
		mat.push_back(mats[i].mat);

		if (mats[i].diffuseMapName != L"")
		{
			ID3D11ShaderResourceView* _diffuseMapSRV = textureMgr.createTexture(texturePath + mats[i].diffuseMapName);
			diffuseMapSRV.push_back(_diffuseMapSRV);
		}
		else
		{
			diffuseMapSRV.push_back(NULL);
		}
	}

	//--------------------------------------------------------
	// Compute bounding box
	//--------------------------------------------------------
	// Compute vertex position extreme values
// 	XMFLOAT3 vMinf3(+MathHelper::infinity, +MathHelper::infinity, +MathHelper::infinity);
// 	XMFLOAT3 vMaxf3(-MathHelper::infinity, -MathHelper::infinity, -MathHelper::infinity);
// 
// 	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
// 	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);
// 
// 	for (int i = 0; i < vertices.size(); ++i)
// 	{
// 		XMVECTOR pos = XMLoadFloat3(&vertices[i].position);
// 
// 		vMin = XMVectorMin(vMin, pos);
// 		vMax = XMVectorMax(vMax, pos);
// 	}
// 
// 	// Store values in bounding box
// 	XMStoreFloat3(&boundingBox.Center, 0.5f*(vMin+vMax));
// 	XMStoreFloat3(&boundingBox.Extents, 0.5f*(vMax-vMin));

	ComputeBoundingAxisAlignedBoxFromPoints(&boundingBox, vertices.size(), &vertices[0].position, sizeof(Vertex::Basic32));
}


BasicModel::~BasicModel(void)
{
}
