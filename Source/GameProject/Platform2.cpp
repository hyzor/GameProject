#include "Platform2.h"

Platform2::Platform2()
	: Platform()
{
	this->mModelName = "Platform2";
}

Platform2::~Platform2()
{
}

void Platform2::Initialize(int id, XMFLOAT3 pos)
{
	Platform::Initialize(id, pos);
}

void Platform2::Update(float dt)
{
	Platform::Update(dt);
}

void Platform2::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap)
{
	Platform::Draw(dc, at, camera, shadowMap);
}

void Platform2::HandleScript()
{
	/* TODO: Code to handle class-specific scripts */
}
