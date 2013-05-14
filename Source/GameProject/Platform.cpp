#include "Platform.h"

Platform::Platform()
	: mEntity(0), mCollision(0), mFilePath("Data\\Models\\Collada\\")
{
}

Platform::~Platform()
{
	for(unsigned int i = 0; i < this->mSwitches.size(); i++)
	{
		SafeDelete(this->mSwitches.at(i));
	}
	SafeDelete(mCollision);
	SafeDelete(mEntity);
}

void Platform::Initialize(int id, XMFLOAT3 pos)
{
	mFilePath += mModelName + "\\";
	this->mID = id;
	this->mEntity = new Entity(GenericHandler::GetInstance()->GetGenericModel(mModelName), pos);
	this->mCollision = new CollisionModel(pos);
	std::string finalPath = mFilePath + mModelName + ".obj";
	this->mCollision->LoadObj(finalPath);
}

void Platform::Move(XMFLOAT3 pos)
{
	mEntity->Position = pos;
	mCollision->SetPosition(pos);
}

void Platform::Update(float dt)
{
	HandleScript();
}

void Platform::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap)
{
	mEntity->Draw(dc, at, camera, shadowMap);
}

PlatformSwitch* Platform::IntersectSwitch(XMVECTOR origin, XMVECTOR dir, float length)
{
	CollisionModel::Hit hit;
	hit.hit = false;
	hit.t = MathHelper::infinity;

	for(unsigned int i = 0; i < mSwitches.size(); ++i)
	{
		CollisionModel::Hit hit2 = mSwitches.at(i)->getCollision()->Intersect(origin, dir, length);
		if(hit2.hit)
		{
			if(hit2.t < hit.t)
			{
				hit = hit2;
				return mSwitches.at(i);
			}
		}
	}
	return NULL;
}
