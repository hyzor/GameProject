#include "Platform4.h"

Platform4::Platform4()
	: Platform()
{
	this->mModelName = "Platform4";
}

Platform4::~Platform4()
{
}

void Platform4::Initialize(int id, XMFLOAT3 pos)
{
	Platform::Initialize(id, pos);
}

void Platform4::Update(float dt)
{
	Platform::Update(dt);
}

void Platform4::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap)
{
	Platform::Draw(dc, at, camera, shadowMap);
}

void Platform4::HandleScript()
{
	/* TODO: Code to handle class-specific scripts */
}
