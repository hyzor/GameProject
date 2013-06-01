#include "Platform.h"

Platform::Platform()
	: mEntity(0), mCollision(0), mFilePath("Data\\Models\\Collada\\"), move(0,0,0), pos(0,0,0)
{
}

Platform::~Platform()
{
	for(unsigned int i = 0; i < this->mSwitches.size(); i++)
		SafeDelete(this->mSwitches.at(i));
	SafeDelete(mCollision);
	SafeDelete(mEntity);
}

void Platform::Initialize(int id, XMFLOAT3 pos)
{
	mFilePath += mModelName + "\\";
	this->mID = id;
	this->mEntity = new Entity(GenericHandler::GetInstance()->GetGenericModel(mModelName), pos);
	this->mCollision = new CollisionModel();
	*this->mCollision = *GenericHandler::GetInstance()->GetCollisionModel(mModelName);
	this->pos = pos;
}

void Platform::Update(float dt)
{
	XMVECTOR pos = XMLoadFloat3(&this->pos);

	pos += XMLoadFloat3(&move)*dt;

	HandleScript();

	XMStoreFloat3(&this->pos, pos);
	mEntity->SetPosition(this->pos);
	mCollision->SetPosition(this->pos);
	for(int i = 0; i < mSwitches.size(); i++)
		mSwitches[i]->SetPosition(this->pos);
}

void Platform::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap)
{
	mEntity->Draw(dc, at, camera, shadowMap);
	for(unsigned int i = 0; i < this->mSwitches.size(); i++)
		this->mSwitches.at(i)->Draw(dc, at, camera, shadowMap);
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

void Platform::HandlePackage(Package* p)
{
	if(p->GetHeader().operation == 6)
	{
		Package::Body b = p->GetBody();
		this->pos = *(XMFLOAT3*)b.Read(4*3);
		this->move = *(XMFLOAT3*)b.Read(4*3);
	}
}

