#include "Pickup.h"


Pickup::Pickup(void)
{
}

Pickup::~Pickup(void)
{
	SafeDelete(mEntity);
}

void Pickup::Initialize(int id, int type, XMFLOAT3 pos)
{
	this->mID = id;
	this->mType = type;
	this->mEntity = new Entity(GenericHandler::GetInstance()->GetGenericModel("Duck"), pos);

	XMMATRIX modelScale = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	XMMATRIX modelRot = XMMatrixRotationY(0.0f);
	XMMATRIX modelOffset = XMMatrixTranslation(mEntity->Position.x, mEntity->Position.y, mEntity->Position.z);
	XMStoreFloat4x4(&mEntity->mInstance.world, modelScale*modelRot*modelOffset);

	UINT vAmount = (UINT)GenericHandler::GetInstance()->GetGenericModel("Duck")->GetVertexCount();
	XNA::ComputeBoundingSphereFromPoints(&mSphere, vAmount, &pos, 3);
}

void Pickup::Update(float dt)
{
}

void Pickup::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap)
{
	mEntity->Draw(dc, at, camera, shadowMap);
}

bool Pickup::Intersect(XMFLOAT3 pos)
{
	XMVECTOR vPos = XMLoadFloat3(&pos);
	return XNA::IntersectPointSphere(vPos, &mSphere);
}

void Pickup::HandlePackage(Package* package)
{
	if(package->GetHeader().operation == 7)
	{
		Package::Body body = package->GetBody();
		this->mPos = *(XMFLOAT3*)body.Read(4*3);
	}
}