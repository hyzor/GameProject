#ifndef COLLISIONMODEL_H
#define COLLISIONMODEL_H


#include <d3dUtilities.h>
#include <D3D11App.h>
#include <Camera.h>
#include <TextureManager.h>
#include <xnacollision.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include "Vertex.h"
#include <string>

#include "GenericModel.h"

class CollisionModel
{
	public:
		struct Hit
		{
			bool hit;
			float t;
		};

		CollisionModel(XMFLOAT3 pos);
		~CollisionModel();

		void LoadObj(char *file);
		int Size();
		XMFLOAT3 *GetPosition(int index);
		void SetPosition(XMFLOAT3 position);
		XNA::AxisAlignedBox GetBoundingBox();
		
		Hit Intersect(XMVECTOR origin, XMVECTOR dir, float length);

		XMVECTOR* vMin;
		XMVECTOR* vMax;

	private:


		std::vector<XMFLOAT3> vertices;
		XMFLOAT3 pos;

		XNA::AxisAlignedBox boundingBox;

		std::vector<std::string> CollisionModel::split(std::string line);
};



#endif