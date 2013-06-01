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
	XMMATRIX modelScale = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	XMMATRIX modelRot = XMMatrixRotationY(0.0f);
	XMMATRIX modelOffset = XMMatrixTranslation(mEntity->Position.x, mEntity->Position.y, mEntity->Position.z);
	XMStoreFloat4x4(&mEntity->mInstance.world, modelScale*modelRot*modelOffset);

	this->mSphere.Center = pos;
	this->mSphere.Radius = 30.0f;
}

void Pickup::Update(float dt, XNA::AxisAlignedBox *player, World* w)
{
	Platform* p = w->getPlatform(this->getCPID());

	if(p != NULL)
	{
		XMFLOAT3 pos = p->getPos();
		Platform* p = w->getPlatform(this->getCPID());
		/*this->mPos.x += pos.x;
		this->mPos.y += pos.y;
		this->mPos.z += pos.z;*/
		XMFLOAT3 pos2 = this->mPos;
		pos2.x += pos.x;
		pos2.y += pos.y;
		pos2.z += pos.z;


		mEntity->Scale = XMFLOAT3(0.1f, 0.1f, 0.1f);
		XMMATRIX modelScale = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		XMMATRIX modelRot = XMMatrixRotationY(0.0f);
		mEntity->Position.x = pos2.x;
		mEntity->Position.y = pos2.y;
		mEntity->Position.z = pos2.z;
		XMMATRIX modelOffset = XMMatrixTranslation(mEntity->Position.x, mEntity->Position.y, mEntity->Position.z);
		XMStoreFloat4x4(&mEntity->mInstance.world, modelScale*modelRot*modelOffset);
		//mEntity->SetPosition(pos2);
		this->mSphere.Center = pos2;
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
