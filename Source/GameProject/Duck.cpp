#include "Duck.h"

Duck::Duck()
	: Platform()
{
	this->mModelName = "Duck";
}

Duck::~Duck()
{
}

void Duck::Initialize(int id, XMFLOAT3 pos)
{
	Platform::Initialize(id, pos);
}

void Duck::Update(float dt)
{
	Platform::Update(dt);
}

void Duck::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap)
{
	Platform::Draw(dc, at, camera, shadowMap);
}

void Duck::HandleScript()
{
	/* TODO: Code to handle class-specific scripts */
}
