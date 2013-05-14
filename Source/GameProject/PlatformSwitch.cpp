#include "PlatformSwitch.h"


PlatformSwitch::PlatformSwitch(void)
{
}

PlatformSwitch::~PlatformSwitch(void)
{
	SafeDelete(mCollision);
	SafeDelete(mEntity);
}

void PlatformSwitch::Initialize(XMFLOAT3 pos)
{
	mEntity = new Entity(GenericHandler::GetInstance()->GetGenericModel("Switch"), pos);
	mCollision = new CollisionModel(pos);
	mCollision->LoadObj("Data\\Models\\Collada\\Switch.obj");
}

void PlatformSwitch::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap)
{
	mEntity->Draw(dc, at, camera, shadowMap);
}

Entity* PlatformSwitch::getEntity() const
{
	return this->mEntity;
}

XMVECTOR PlatformSwitch::GetMoveTo(XMVECTOR up)
{
	if(XMVectorGetY(up) > 0.5f)
		return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(0,-7,-7));
	else if(XMVectorGetZ(up) < 0.5f)
		return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(0, 7, 7));

	return XMLoadFloat3(&XMFLOAT3(0,0,0));
}

float PlatformSwitch::GetRotationX(XMVECTOR up)
{
	if(XMVectorGetY(up) > 0.5f)
		return -(float)XM_PI/2;
	else if(XMVectorGetZ(up) < 0.5f)
		return +(float)XM_PI/2;

	return 0;
}
float PlatformSwitch::GetRotationZ(XMVECTOR up)
{
	return 0;
}