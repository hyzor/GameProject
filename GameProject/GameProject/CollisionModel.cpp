#include "CollisionModel.h"

CollisionModel::CollisionModel(XMFLOAT3 p)
{
	vertices = std::vector<XMFLOAT3>();
	this->pos = p;
	this->vMin = new XMVECTOR();
	this->vMax = new XMVECTOR();
}

CollisionModel::~CollisionModel()
{
	SafeDelete(vMin);
	SafeDelete(vMax);
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

	XMFLOAT3 vMinf3(+MathHelper::infinity, +MathHelper::infinity, +MathHelper::infinity);
	XMFLOAT3 vMaxf3(-MathHelper::infinity, -MathHelper::infinity, -MathHelper::infinity);

	 *vMin = XMLoadFloat3(&vMinf3);
	 *vMax = XMLoadFloat3(&vMaxf3);

	for (UINT i = 0; i < Size(); i++)
	{
		XMVECTOR pos = XMLoadFloat3(GetPosition(i));

		*vMin = XMVectorMin(*vMin, pos);
		*vMax = XMVectorMax(*vMax, pos);
	}

	SetPosition(pos);
}

int CollisionModel::Size()
{
	return vertices.size();
}

XMFLOAT3 *CollisionModel::GetPosition(int index)
{
	return &vertices[index];
}

void CollisionModel::SetPosition(XMFLOAT3 position)
{
	pos = position;
	XMStoreFloat3(&boundingBox.Center, XMLoadFloat3(&pos)+0.5f*(*vMin+*vMax));
	XMStoreFloat3(&boundingBox.Extents, 0.5f*(*vMax-*vMin));
}

XNA::AxisAlignedBox CollisionModel::GetBoundingBox()
{
	return boundingBox;
}

CollisionModel::Hit CollisionModel::Intersect(XMVECTOR origin, XMVECTOR dir, float length)
{
	origin *= XMLoadFloat3(&XMFLOAT3(1,1,-1)); //transform problem
	
	Hit h;
	h.hit = false;
	h.t = 100000;
	float t;
	if(XNA::IntersectRayAxisAlignedBox(origin,dir, &GetBoundingBox(), &t) && t < length)
	{
		for(int i = 0; i < Size(); i+=3)
		{
			XMVECTOR v0 = XMLoadFloat3(GetPosition(i+0));
			XMVECTOR v1 = XMLoadFloat3(GetPosition(i+1));
			XMVECTOR v2 = XMLoadFloat3(GetPosition(i+2));
			if (XNA::IntersectRayTriangle(origin, dir, v0, v1, v2, &t) && t < length)
			{
				if(t < h.t)
				{
					h.hit = true;
					h.t = t;
				}
			}
		}
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