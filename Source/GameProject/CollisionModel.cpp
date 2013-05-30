#include "CollisionModel.h"

CollisionModel::CollisionModel()
{
	vertices = std::vector<XMFLOAT3>();
	this->pos = XMFLOAT3(0,0,0);
	this->shared = false;
}

CollisionModel::CollisionModel(XMFLOAT3 p)
{
	vertices = std::vector<XMFLOAT3>();
	this->pos = p;
	this->shared = false;
}

CollisionModel::CollisionModel(bool shared)
{
	vertices = std::vector<XMFLOAT3>();
	this->pos = XMFLOAT3(0,0,0);
	this->shared = shared;
}

CollisionModel::~CollisionModel()
{
	if(!shared)
		SafeDelete(SplitTree);
}

void CollisionModel::LoadObj(std::string fileName)
{
	this->LoadObj(fileName, XMMatrixIdentity());
}

void CollisionModel::LoadObj(std::string fileName, const XMMATRIX &matrix)
{
	std::vector<XMFLOAT3> positions;

	std::ifstream infile(fileName);
	std::string line;
	while (std::getline(infile, line))
	{
		std::vector<std::string> elements = split(line);
		if(elements.size() > 0)
		{
			if(elements.at(0)=="v" )
				positions.push_back(XMFLOAT3((float)atof(elements.at(1).c_str()), (float)atof(elements.at(2).c_str()), (float)atof(elements.at(3).c_str())));
			else if(elements.at(0)=="f" && elements.size() > 6)
			{
				XMFLOAT3 p;
				XMStoreFloat3(&p, XMVector3Transform(XMLoadFloat3(&positions.at(atoi(elements.at(1).c_str())-1)), matrix));
				vertices.push_back(p);
				XMStoreFloat3(&p, XMVector3Transform(XMLoadFloat3(&positions.at(atoi(elements.at(4).c_str())-1)), matrix));
				vertices.push_back(p);
				XMStoreFloat3(&p, XMVector3Transform(XMLoadFloat3(&positions.at(atoi(elements.at(7).c_str())-1)), matrix));
				vertices.push_back(p);
			}
		}
	}
	infile.close();

	XMFLOAT3 vMinf3(+MathHelper::infinity, +MathHelper::infinity, +MathHelper::infinity);
	XMFLOAT3 vMaxf3(-MathHelper::infinity, -MathHelper::infinity, -MathHelper::infinity);

	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

	for (int i = 0; i < Size(); i++)
	{
		XMVECTOR pos = XMLoadFloat3(GetPosition(i));

		vMin = XMVectorMin(vMin, pos);
		vMax = XMVectorMax(vMax, pos);
	}

	XMStoreFloat3(&this->vMin, vMin);
	XMStoreFloat3(&this->vMax, vMax);

	SplitTree = new SplitNodeParent(vMin, vMax, 0);
	for(int i = 0; i < Size(); i+=3)
		SplitTree->Add(GetPosition(i+0), GetPosition(i+1), GetPosition(i+2));
	SplitTree->CleanUp();
	

	SetPosition(pos);
}

int CollisionModel::Size()
{
	return vertices.size();
}

XMFLOAT3 *CollisionModel::GetPosition(int index)
{
	return &vertices.at(index);
}

void CollisionModel::SetPosition(XMFLOAT3 position)
{
	pos = position;
	XMStoreFloat3(&boundingBox.Center, XMLoadFloat3(&pos)*XMLoadFloat3(&XMFLOAT3(1,1,-1))+XMLoadFloat3(&vMin)+0.5f*(XMLoadFloat3(&vMax)-XMLoadFloat3(&vMin)));
	XMStoreFloat3(&boundingBox.Extents, 0.5f*(XMLoadFloat3(&vMax)-XMLoadFloat3(&vMin)));
}

void CollisionModel::SetPosition(XMVECTOR position)
{
	XMFLOAT3 p;
	XMStoreFloat3(&p, position);
	this->SetPosition(p);
}


CollisionModel::Hit CollisionModel::Intersect(XMVECTOR origin, XMVECTOR dir, float length)
{
	origin *= XMLoadFloat3(&XMFLOAT3(1,1,-1)); //transform problem
	dir *= XMLoadFloat3(&XMFLOAT3(1,1,-1)); //transform problem
	
	Hit h;
	h.hit = false;
	h.t = 100000;
	float t;
	XMVECTOR npos = XMLoadFloat3(&pos)*XMLoadFloat3(&XMFLOAT3(1,1,-1));
	if(XNA::IntersectRayAxisAlignedBox(origin, dir, &boundingBox, &t) && t < length)
		h = SplitTree->Intersects(&(origin-npos), &dir, length);

	return h;
}



std::vector<std::string> CollisionModel::split(std::string line)
{
	std::vector<std::string> elements;
	std::string element = "";
	for(int i = 0; i < (int)line.length(); i++)
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









CollisionModel::Plane::Plane(XMFLOAT3 pos, XMFLOAT3 dir)
{
	this->pos = pos;
	this->dir = dir;
}

bool CollisionModel::Plane::intersectsRay(XMVECTOR* origin, XMVECTOR* dir, float length, float flip)
{
	XMVECTOR newPos = XMLoadFloat3(&pos);
	XMVECTOR v1 = *origin - newPos;
	if(XMVectorGetX(XMVector3Dot(v1, XMLoadFloat3(&this->dir)))*flip < 0)
		return true;
	v1 += *dir * length;
	if(XMVectorGetX(XMVector3Dot(v1, XMLoadFloat3(&this->dir)))*flip < 0)
		return true;
	return false;
}

bool CollisionModel::Plane::intersectsTriangle(XMFLOAT3* v0, XMFLOAT3* v1, XMFLOAT3* v2, float flip)
{
	XMVECTOR newPos = XMLoadFloat3(&this->pos);
	if(XMVectorGetX(XMVector3Dot(XMLoadFloat3(v0)-newPos, XMLoadFloat3(&this->dir)))*flip < 0)
		return true;
	if(XMVectorGetX(XMVector3Dot(XMLoadFloat3(v1)-newPos, XMLoadFloat3(&this->dir)))*flip < 0)
		return true;
	if(XMVectorGetX(XMVector3Dot(XMLoadFloat3(v2)-newPos, XMLoadFloat3(&this->dir)))*flip < 0)
		return true;
	return false;
}


CollisionModel::SplitNode::SplitNode(XMVECTOR vMin, XMVECTOR vMax)
{
	XMVECTOR center = (vMin+vMax)*0.5f;
	XMVECTOR size = vMax-vMin;

	XMFLOAT3 c;
	XMStoreFloat3(&c,center);

	if(XMVectorGetX(size) > XMVectorGetY(size) && XMVectorGetX(size) > XMVectorGetZ(size))
		plane = new Plane(c, XMFLOAT3(1, 0, 0));
	else if(XMVectorGetY(size) > XMVectorGetZ(size))
		plane = new Plane(c, XMFLOAT3(0, 1, 0));
	else
		plane = new Plane(c, XMFLOAT3(0, 0, 1));
}

CollisionModel::SplitNode::~SplitNode()
{
	SafeDelete(plane);
}

bool CollisionModel::SplitNode::HasGeometry()
{
	return false;
}

void CollisionModel::SplitNode::Add(XMFLOAT3* v0, XMFLOAT3* v1, XMFLOAT3* v2){}

void CollisionModel::SplitNode::CleanUp(){}

CollisionModel::Hit CollisionModel::SplitNode::Intersects(XMVECTOR* origin, XMVECTOR* dir, float length)
{
	CollisionModel::Hit h;
	h.hit = false;
	h.t = MathHelper::infinity;
	return h;
}

CollisionModel::SplitNodeParent::SplitNodeParent(XMVECTOR vMin, XMVECTOR vMax, int layer) : CollisionModel::SplitNodeParent::SplitNode(vMin, vMax)
{
	this->left = NULL;
	this->right = NULL;

	XMVECTOR size = vMax-vMin;
	if(layer > 0)
	{
		left = new SplitNodeParent(vMin, vMax-size*XMLoadFloat3(&plane->dir)*0.5f, layer-1);
		right = new SplitNodeParent(vMin+size*XMLoadFloat3(&plane->dir)*0.5f, vMax, layer-1);
	}
	else
	{
		left = new SplitNodeLeaf(vMin, vMax-size*XMLoadFloat3(&plane->dir)*0.5f);
		right = new SplitNodeLeaf(vMin+size*XMLoadFloat3(&plane->dir)*0.5f, vMax);
	}
}

CollisionModel::SplitNodeParent::~SplitNodeParent()
{
	SafeDelete(plane);
	SafeDelete(left);
	SafeDelete(right);
}

bool CollisionModel::SplitNodeParent::HasGeometry()
{
	if(left != NULL) if(left->HasGeometry()) return true;
	if(right != NULL) if(right->HasGeometry()) return true;
	return false;
}

void CollisionModel::SplitNodeParent::Add(XMFLOAT3* v0, XMFLOAT3* v1, XMFLOAT3* v2)
{
	if(plane->intersectsTriangle(v0, v1, v2, 1))
		left->Add(v0, v1, v2);
	if(plane->intersectsTriangle(v0, v1, v2, -1))
		right->Add(v0, v1, v2);
}

void CollisionModel::SplitNodeParent::CleanUp()
{
	if(!left->HasGeometry()) SafeDelete(left);
	if(!right->HasGeometry()) SafeDelete(right);
}

CollisionModel::Hit CollisionModel::SplitNodeParent::Intersects(XMVECTOR* origin, XMVECTOR* dir, float length)
{
	CollisionModel::Hit h1;
	h1.hit = false;
	if(left != NULL && plane->intersectsRay(origin, dir, length, 1))
		h1 = left->Intersects(origin, dir, length);
	CollisionModel::Hit h2;
	h2.hit = false;
	if(right != NULL && plane->intersectsRay(origin, dir, length, -1))
		h2 = right->Intersects(origin, dir, length);

	if(h1.hit)
	{
		if(h2.hit)
		{
			if(h2.t < h1.t)
				return h2;
			return h1;
		}
		return h1;
	}
	return h2;
}


CollisionModel::SplitNodeLeaf::SplitNodeLeaf(XMVECTOR vMin, XMVECTOR vMax) : CollisionModel::SplitNodeLeaf::SplitNode(vMin, vMax){}

CollisionModel::SplitNodeLeaf::~SplitNodeLeaf()
{
	SafeDelete(plane);
}

bool CollisionModel::SplitNodeLeaf::HasGeometry()
{
	return left.size() > 0 || right.size() > 0;
}

void CollisionModel::SplitNodeLeaf::Add(XMFLOAT3* v0, XMFLOAT3* v1, XMFLOAT3* v2)
{
	if(plane->intersectsTriangle(v0, v1, v2, 1))
	{
		left.push_back(v0);
		left.push_back(v1);
		left.push_back(v2);
	}
	if(plane->intersectsTriangle(v0, v1, v2, -1))
	{
		right.push_back(v0);
		right.push_back(v1);
		right.push_back(v2);
	}
}

void CollisionModel::SplitNodeLeaf::CleanUp(){}

CollisionModel::Hit CollisionModel::SplitNodeLeaf::Intersects(XMVECTOR* origin, XMVECTOR* dir, float length)
{

	float t;
	CollisionModel::Hit h;
	h.hit = false;
	h.t = MathHelper::infinity;

	if(plane->intersectsRay(origin, dir, length, 1))
		for(int i = 0; i < (int)left.size(); i+=3)
		{
			XMVECTOR v0 = XMLoadFloat3(left[i+0]);
			XMVECTOR v1 = XMLoadFloat3(left[i+1]);
			XMVECTOR v2 = XMLoadFloat3(left[i+2]);
			if (XNA::IntersectRayTriangle(*origin, *dir, v0, v1, v2, &t) && t < length)
				if(t < h.t)
				{
					h.hit = true;
					h.t = t;
				}
		}

	if(plane->intersectsRay(origin, dir, length, -1))
		for(int i = 0; i < (int)right.size(); i+=3)
		{
			XMVECTOR v0 = XMLoadFloat3(right[i+0]);
			XMVECTOR v1 = XMLoadFloat3(right[i+1]);
			XMVECTOR v2 = XMLoadFloat3(right[i+2]);
			if (XNA::IntersectRayTriangle(*origin, *dir, v0, v1, v2, &t) && t < length)
				if(t < h.t)
				{
					h.hit = true;
					h.t = t;
				}
		}

	
	return h;
}