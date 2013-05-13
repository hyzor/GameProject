#include "Platform1.h"

Platform1::Platform1()
	: Platform()
{
	this->mModelName = "Platform1";
}

Platform1::~Platform1()
{
}

void Platform1::Initialize(int id, XMFLOAT3 pos)
{
	Platform::Initialize(id, pos);
}

void Platform1::Update(float dt)
{
	Platform::Update(dt);
}

void Platform1::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap)
{
	Platform::Draw(dc, at, camera, shadowMap);
}

void Platform1::HandleScript()
{
	/* TODO: Code to handle class-specific scripts */
}
