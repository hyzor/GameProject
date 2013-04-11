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

#include "GenericModel.h"

class CollisionModel
{
	public:
		struct Hit
		{
			bool hit;
			float t;
			float dot;
		};

		CollisionModel(GenericModel* model, XMFLOAT3 pos);
		
		Hit Intersect(XMVECTOR origin, XMVECTOR dir);

	private:
		GenericModel* model;
		XMFLOAT3 pos;
};



#endif