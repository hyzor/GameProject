#include "CollisionModel.h"

CollisionModel::CollisionModel(GenericModel* model, XMFLOAT3 p)
{
	this->model = model;
	this->pos = p;
}

CollisionModel::Hit CollisionModel::Intersect(XMVECTOR origin, XMVECTOR dir)
{
	origin -= XMLoadFloat3(&pos);

	Hit h;
	h.hit = false;
	for(int i = 0; i < model->GetVertexCount(); i+=3)
	{
		//move to origin
		XMVECTOR f = XMLoadFloat3(&model->GetVertex(i+0)->position);
		XMVECTOR o = origin - f;
		XMVECTOR d1 = XMLoadFloat3(&model->GetVertex(i+1)->position) - f;
		XMVECTOR d2 = XMLoadFloat3(&model->GetVertex(i+2)->position) - f;

		//find plane normal
		XMVECTOR n = XMVector3Cross(d1, d2);

		//calculate plane intersection
		float t2 = -XMVectorGetX(XMVector3Dot(o,n)/XMVector3Dot(dir,n));

		if(t2 > 0.0f && t2 < 1)
		{
			//intersection point
			XMVECTOR p = origin + dir*t2;

			float d = XMVectorGetX(XMVector3Dot(d1,d2) * XMVector3Dot(d1,d2) - XMVector3Dot(d1,d1) * XMVector3Dot(d2,d2));

			//Test intersection
			float s = XMVectorGetX(XMVector3Dot(d1,d2) * XMVector3Dot(p,d2) - XMVector3Dot(d2,d2) * XMVector3Dot(p,d1)) / d;
			if (s < 0.0 || s > 1.0) 
				continue;
			float t3 = XMVectorGetX(XMVector3Dot(d1,d2) * XMVector3Dot(p,d1) - XMVector3Dot(d1,d1) * XMVector3Dot(p,d2)) / d;
			if (t3 < 0.0 || s + t3 > 1.0)
				continue;


			if(!h.hit || t2 < h.t)
			{
				h.hit = true;
				h.t = t2;
				h.dot = XMVectorGetX(XMVector3Dot(dir,n));
			}
		}
	}

	return h;
}