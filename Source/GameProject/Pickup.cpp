#include "Pickup.h"


Pickup::Pickup(void)
{
}

Pickup::~Pickup(void)
{
	SafeDelete(mEntity);
}

void Pickup::Initialize(int id, int type, int cpID, XMFLOAT3 pos)
{
	this->mID = id;
	this->cpID = cpID;
	this->mPos = pos;
	this->mType = type;
	this->mEntity = new Entity(GenericHandler::GetInstance()->GetGenericModel("Duck"), pos);

	mEntity->Scale = XMFLOAT3(0.1f, 0.1f, 0.1f);

	this->mSphere.Center = pos;
	this->mSphere.Radius = 30.0f;
}

void Pickup::Update(float dt, XNA::AxisAlignedBox *player, World* w)
{
	Platform* p = w->getPlatform(this->getCPID());

	if(p != NULL)
	{
		XMFLOAT3 pos = p->getPos();
		pos.x += this->mPos.x;
		pos.y += this->mPos.y;
		pos.z += this->mPos.z;

		mEntity->SetPosition(pos);
		this->mSphere.Center = pos;
	}

	if(XNA::IntersectSphereAxisAlignedBox(&mSphere, player))
	{
		struct Data
		{
			int id; //dummy
		};
		Data* data = new Data;
		data->id = this->mID;

		Network::GetInstance()->Push(new Package(Package::Header(9, this->mID, sizeof(Data)), Package::Body((char*)(data))));
	}
}

void Pickup::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap)
{
	mEntity->Draw(dc, at, camera, shadowMap);
}
