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

		void LoadObj(std::string fileName);
		void LoadObj(std::string fileName, const XMMATRIX &matrix);

		int Size();
		XMFLOAT3 *GetPosition(int index);
		void SetPosition(XMFLOAT3 position);
		
		Hit Intersect(XMVECTOR origin, XMVECTOR dir, float length);

		XMVECTOR* vMin;
		XMVECTOR* vMax;

	private:

		std::vector<XMFLOAT3> vertices;
		XMFLOAT3 pos;

		XNA::AxisAlignedBox boundingBox;

		std::vector<std::string> CollisionModel::split(std::string line);

		
		class Plane
		{
			public:
				Plane(XMFLOAT3 pos, XMFLOAT3 dir);
				bool intersectsTriangle(XMFLOAT3* v0, XMFLOAT3* v1, XMFLOAT3* v2, float flip);
				bool intersectsRay(XMVECTOR* origin, XMVECTOR* dir, float length, float flip);

				XMFLOAT3 dir;
				XMFLOAT3 pos;
		};

		class SplitNode
		{
			public:
				SplitNode(XMVECTOR vMin, XMVECTOR vMax);
				virtual ~SplitNode();
				virtual bool HasGeometry();
				virtual void Add(XMFLOAT3* v0, XMFLOAT3* v1, XMFLOAT3* v2);
				virtual void CleanUp();
				virtual Hit Intersects(XMVECTOR* origin, XMVECTOR* dir, float length);
			protected:
				Plane* plane;
		};

		class SplitNodeParent : public SplitNode
		{
			public:
				SplitNodeParent(XMVECTOR vMin, XMVECTOR vMax, int layer);
				~SplitNodeParent();
				bool HasGeometry();
				void Add(XMFLOAT3* v0, XMFLOAT3* v1, XMFLOAT3* v2);
				void CleanUp();
				Hit Intersects(XMVECTOR* origin, XMVECTOR* dir, float length);
			private:
				SplitNode *left;
				SplitNode *right;
		};

		class SplitNodeLeaf : public SplitNode
		{
			public:
				SplitNodeLeaf(XMVECTOR vMin, XMVECTOR vMax);
				~SplitNodeLeaf();
				bool HasGeometry();
				void Add(XMFLOAT3* v0, XMFLOAT3* v1, XMFLOAT3* v2);
				void CleanUp();
				Hit Intersects(XMVECTOR* origin, XMVECTOR* dir, float length);
			private:
				std::vector<XMFLOAT3*> left;
				std::vector<XMFLOAT3*> right;
		};

		SplitNodeParent* SplitTree;
};



#endif