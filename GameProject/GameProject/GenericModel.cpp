#include "GenericModel.h"


GenericModel::GenericModel(ID3D11Device* device, TextureManager& textureMgr, const std::string& fileName, const std::wstring& texturePath)
{
	std::vector<GenericMaterial> mats;

	// Load all the meshes
	GenericObjectLoader objectLoader;
	//objectLoader.loadObject(fileName, mats, meshes, device);
	objectLoader.loadObject(fileName, mats, meshes);

	meshCount = meshes.size();
	materialCount = mats.size();

	// Set vertices and indices for each mesh
	for (UINT i = 0; i < meshCount; ++i)
	{
		meshes[i].setIndices(device, &meshes[i].indices[0], meshes[i].indices.size());
		meshes[i].setVertices(device, &meshes[i].vertices[0], meshes[i].vertices.size());

		// Push back addresses to all the vertices to the models vector of pointers
		for (UINT j = 0; j < meshes[i].vertices.size(); ++j)
			vertices.push_back(&meshes[i].vertices[j]);
	}

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

		if (mats[i].normalMapName != L"")
		{
			ID3D11ShaderResourceView* _normalMapSRV = textureMgr.createTexture(texturePath + mats[i].normalMapName);
			normalMapSRV.push_back(_normalMapSRV);
		}
		else
		{
			normalMapSRV.push_back(NULL);
		}
	}

	//--------------------------------------------------------
	// Compute bounding box
	//--------------------------------------------------------
	// Compute vertex position extreme values
	XMFLOAT3 vMinf3(+MathHelper::infinity, +MathHelper::infinity, +MathHelper::infinity);
	XMFLOAT3 vMaxf3(-MathHelper::infinity, -MathHelper::infinity, -MathHelper::infinity);

	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

	for (UINT i = 0; i < meshCount; ++i)
	{
		for (UINT j = 0; j < meshes[i].vertices.size(); ++j)
		{
			XMVECTOR pos = XMLoadFloat3(&meshes[i].vertices[j].position);

			vMin = XMVectorMin(vMin, pos);
			vMax = XMVectorMax(vMax, pos);
		}
	}

// 	for (int i = 0; i < vertices.size(); ++i)
// 	{
// 		XMVECTOR pos = XMLoadFloat3(&vertices[i].position);
// 
// 		vMin = XMVectorMin(vMin, pos);
// 		vMax = XMVectorMax(vMax, pos);
// 	}

	// Store values in bounding box
	XMStoreFloat3(&boundingBox.Center, 0.5f*(vMin+vMax));
	XMStoreFloat3(&boundingBox.Extents, 0.5f*(vMax-vMin));

	//ComputeBoundingAxisAlignedBoxFromPoints(&boundingBox, vertices.size(), &vertices[0]->position, sizeof(Vertex::Basic32));
}


GenericModel::~GenericModel(void)
{
}
