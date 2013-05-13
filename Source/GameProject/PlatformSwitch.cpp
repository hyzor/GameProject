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
