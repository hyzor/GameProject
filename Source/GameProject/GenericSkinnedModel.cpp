#include "GenericSkinnedModel.h"


GenericSkinnedModel::GenericSkinnedModel(ID3D11Device* device, TextureManager& textureMgr, const std::string& fileName, const std::wstring& texturePath)
{
	std::vector<GenericMaterial> mats;

	// Load all the meshes
	GenericObjectLoader objectLoader;
	objectLoader.loadSkinnedObject(fileName, mats, meshes, skinnedData);

	numMeshes = meshes.size();
	numMaterials = mats.size();

	// Set vertices and indices for each mesh
	for (UINT i = 0; i < numMeshes; ++i)
	{
		meshes[i].setIndices(device, &meshes[i].mIndices[0], meshes[i].mIndices.size());
		meshes[i].setVertices(device, &meshes[i].mVertices[0], meshes[i].mVertices.size());
	}

	for (UINT i = 0; i < numMaterials; ++i)
	{
		mat.push_back(mats[i].mat);

		if (mats[i].diffuseMapName != L"")
		{
			ID3D11ShaderResourceView* _diffuseMapSRV = textureMgr.CreateTexture(texturePath + mats[i].diffuseMapName);
			diffuseMapSRV.push_back(_diffuseMapSRV);
		}
		else
		{
			diffuseMapSRV.push_back(NULL);
		}

		if (mats[i].normalMapName != L"")
		{
			ID3D11ShaderResourceView* _normalMapSRV = textureMgr.CreateTexture(texturePath + mats[i].normalMapName);
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

	for (UINT i = 0; i < numMeshes; ++i)
	{
		for (UINT j = 0; j < meshes[i].mVertices.size(); ++j)
		{
			XMVECTOR pos = XMLoadFloat3(&meshes[i].mVertices[j].position);

			vMin = XMVectorMin(vMin, pos);
			vMax = XMVectorMax(vMax, pos);
		}
	}

	// Store values in bounding box
	XMStoreFloat3(&boundingBox.Center, 0.5f*(vMin+vMax));
	XMStoreFloat3(&boundingBox.Extents, 0.5f*(vMax-vMin));

	//ComputeBoundingAxisAlignedBoxFromPoints(&boundingBox, vertices.size(), &vertices[0]->position, sizeof(Vertex::Basic32));
}


GenericSkinnedModel::~GenericSkinnedModel(void)
{
}

void GenericSkinnedModelInstance::Update(float dt)
{
	TimePos += dt;

	FinalTransforms = model->skinnedData.GetTransforms(TimePos, AnimationIndex, frameStart, frameEnd, playAnimForward);
}