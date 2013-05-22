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

	int index = 0;
	XMFLOAT3 switchPos = pos;
	XMFLOAT4X4 rot;

	switchPos.x += 44.448f;
	switchPos.y += -51.658f;
	switchPos.z += -108.818f;
	
	XMStoreFloat4x4(&rot, XMMatrixIdentity());
	PlatformSwitch* ps = new PlatformSwitch();
	this->mSwitches.push_back(ps);
	this->mSwitches.at(index)->Initialize(switchPos, 3, 11, rot, XMFLOAT3(5.0f, 0.0f, -5.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityY(-XM_PI/2);
	index++;

	/////////////////////////////////////////////////////////////////////

	PlatformSwitch* ps1 = new PlatformSwitch();
	switchPos = pos;
	switchPos.x += 44.442f;
	switchPos.y += -78.477f;
	switchPos.z += -16.483f;

	this->mSwitches.push_back(ps1);
	this->mSwitches.at(index)->Initialize(switchPos, 2, 12, rot, XMFLOAT3(5.0f, -5.0f, 0));
	this->mSwitches.at(index)->getEntity()->RotateEntityZ(-XM_PI/2);
	index++;

	/////////////////////////////////////////////////////////7

	PlatformSwitch* ps2 = new PlatformSwitch();
	switchPos = pos;
	switchPos.x += 3.383f;
	switchPos.y += -78.315f;
	switchPos.z += 108.886f;

	this->mSwitches.push_back(ps2);
	this->mSwitches.at(index)->Initialize(switchPos, 1, 3, rot, XMFLOAT3(0, -10.0f, 5.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityX(XM_PI);
	index++;

	///////////////////////////////////////////////////////

	//ok
	PlatformSwitch* ps3 = new PlatformSwitch();
	switchPos = pos;
	switchPos.x += -44.568f;
	switchPos.y += -44.145f;
	switchPos.z += 108.701f;

	this->mSwitches.push_back(ps3);
	this->mSwitches.at(index)->Initialize(switchPos, 3, 10, rot, XMFLOAT3(-5.0f, 0.0f, 5.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityY(XM_PI/2);
	index++;

	////////////////////////////////////////////////////

	//ok
	PlatformSwitch* ps4 = new PlatformSwitch();
	switchPos = pos;
	switchPos.x += -32.571f;
	switchPos.y += -24.59f;
	switchPos.z += -30.935f;

	this->mSwitches.push_back(ps4);
	this->mSwitches.at(index)->Initialize(switchPos, 2, 6, rot, XMFLOAT3(-5.0f, 0.0f, 0.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityZ(XM_PI/2);
	index++;

	//////////////////////////////////////////////////////

	//ok
	PlatformSwitch* ps5 = new PlatformSwitch();
	switchPos = pos;
	switchPos.x += -12.918f;
	switchPos.y += -12.54f;
	switchPos.z += -108.889f;

	this->mSwitches.push_back(ps5);
	this->mSwitches.at(index)->Initialize(switchPos, 1, 1, rot, XMFLOAT3(0, 0, 0));
	index++;
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
