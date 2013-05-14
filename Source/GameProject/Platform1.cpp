#include "Platform1.h"

Platform1::Platform1()
	: Platform()
{
	//this->mFilePath = "Data\\Models\\Collada\\Platform1\\";
	this->mModelName = "Platform1";
}

Platform1::~Platform1()
{
}

void Platform1::Initialize(int id, XMFLOAT3 pos)
{
	Platform::Initialize(id, pos);

	XMFLOAT3 switchPos = pos;
	/*switchPos.y += 100.0;
	switchPos.x += 50.0f;
	switchPos.z += 72.5f;*/
	switchPos.y += 16.8;
	switchPos.x += 0.0f;
	switchPos.z += -78.8f;

	PlatformSwitch* ps = new PlatformSwitch();

	this->mSwitches.push_back(ps);
	this->mSwitches.at(0)->Initialize(switchPos);
	//this->mSwitches.at(0)->getEntity()->RotateEntityX(XM_PI/2);
}

void Platform1::Update(float dt)
{
	Platform::Update(dt);
}

void Platform1::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap)
{
	Platform::Draw(dc, at, camera, shadowMap);

	for(unsigned int i = 0; i < this->mSwitches.size(); i++)
	{
		this->mSwitches.at(i)->Draw(dc, at, camera, shadowMap);
	}
}

void Platform1::HandleScript()
{
	/* TODO: Code to handle class-specific scripts */
}
