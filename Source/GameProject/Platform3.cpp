#include "Platform3.h"

Platform3::Platform3()
	: Platform()
{
	this->mModelName = "Platform3";
}

Platform3::~Platform3()
{
}

void Platform3::Initialize(int id, XMFLOAT3 pos)
{
	Platform::Initialize(id, pos);
}

void Platform3::Update(float dt)
{
	Platform::Update(dt);
}

void Platform3::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap)
{
	Platform::Draw(dc, at, camera, shadowMap);
}

void Platform3::HandleScript()
{
	/* TODO: Code to handle class-specific scripts */
}
