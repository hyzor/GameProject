#include "CollisionModel.h"

CollisionModel::CollisionModel(XMFLOAT3 p)
{
	vertices = std::vector<XMFLOAT3>();
	this->pos = p;
}

void CollisionModel::LoadObj(char *file)
{
	std::vector<XMFLOAT3> positions;

	std::ifstream infile(file);
	std::string line;
	while (std::getline(infile, line))
	{
		std::vector<std::string> elements = split(line);
		if(elements.size() > 0)
		{
			if(elements.at(0)=="v" )
				positions.push_back(XMFLOAT3(atof(elements.at(1).c_str()), atof(elements.at(2).c_str()), atof(elements.at(3).c_str())));
			else if(elements.at(0)=="f" && elements.size() > 6)
			{
				vertices.push_back(positions.at(atof(elements.at(1).c_str())-1));
				vertices.push_back(positions.at(atof(elements.at(4).c_str())-1));
				vertices.push_back(positions.at(atoi(elements.at(7).c_str())-1));
			}
		}
	}
	infile.close();
}

int CollisionModel::Size()
{
	return vertices.size();
}

XMFLOAT3 *CollisionModel::GetPosition(int index)
{
	return &vertices[index];
}

CollisionModel::Hit CollisionModel::Intersect(XMVECTOR origin, XMVECTOR dir)
{
	//origin -= XMLoadFloat3(&pos);

	origin *= XMLoadFloat3(&XMFLOAT3(1,1,-1)); //transform problem

	Hit h;
	h.hit = false;
	h.t = 100000;
	for(int i = 0; i < Size(); i+=3)
	{
		float t;
		XMVECTOR v0 = XMLoadFloat3(GetPosition(i+0));
		XMVECTOR v1 = XMLoadFloat3(GetPosition(i+1));
		XMVECTOR v2 = XMLoadFloat3(GetPosition(i+2));
		if (XNA::IntersectRayTriangle(origin, dir, v0, v1, v2, &t))
		{
			if(t < h.t)
			{
				h.hit = true;
				h.dot = 0;
				h.t = t;
			}
		}

		/*if (XNA::IntersectRayTriangle(origin, dir, v2, v1, v0, &t))
		{
			if(t < h.t)
			{
				h.hit = true;
				h.dot = 0;
				h.t = t;
			}
		}*/
		/*
		//move to origin
		XMVECTOR f = XMLoadFloat3(GetPosition(i+0));
		XMVECTOR o = origin - f;
		XMVECTOR d1 = XMLoadFloat3(GetPosition(i+1)) - f;
		XMVECTOR d2 = XMLoadFloat3(GetPosition(i+2)) - f;
		XNA::IntersectRayTriangle(
		//find plane normal
		XMVECTOR n =  XMVector3Cross(d1, d2);//XMLoadFloat3(&GetPosition(i+0)->normal);

		//calculate plane intersection
		float t2 = -XMVectorGetX(XMVector3Dot(o,n)/XMVector3Dot(dir,n));

		if(t2 > 0 && t2 < 1)
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
				h.t = (t2 < 0) ? -t2 : t2;
				h.dot = XMVectorGetX(XMVector3Dot(dir,n));
			}
		}
		*/
	}

	return h;
}



std::vector<std::string> CollisionModel::split(std::string line)
{
	std::vector<std::string> elements;
	std::string element = "";
	for(int i = 0; i < line.length(); i++)
	{
		if(line[i] != ' ' && line[i] != '/')
			element += line[i];
		else
		{
			elements.push_back(element);
			element = "";
		}
	}
	elements.push_back(element);
	
	return elements;
}