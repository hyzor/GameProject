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

	int index = 0;

	XMFLOAT3 switchPos = pos;
	XMFLOAT4X4 rot;

	switchPos.x += -129.8f;
	switchPos.y += -0.95f;
	switchPos.z += -91.9f;
	
	XMStoreFloat4x4(&rot, XMMatrixIdentity());
	PlatformSwitch* ps = new PlatformSwitch();
	this->mSwitches.push_back(ps);
	this->mSwitches.at(index)->Initialize(switchPos, 3, 8, rot, XMFLOAT3(0, 0, 0));
	index++;

	/////////////////////////////////////////////////////////////////////

	PlatformSwitch* ps1 = new PlatformSwitch();
	switchPos = pos;
	switchPos.x += 8.64f;
	switchPos.y += -48.578f;
	switchPos.z += -91.851f;

	this->mSwitches.push_back(ps1);
	this->mSwitches.at(index)->Initialize(switchPos, 1, 4, rot, XMFLOAT3(0, -5.0f, 0.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityX(-XM_PI/2);
	index++;

	/////////////////////////////////////////////////////////7

	PlatformSwitch* ps2 = new PlatformSwitch();
	switchPos = pos;
	switchPos.x += -33.338f;
	switchPos.y += -48.542f;
	switchPos.z += 91.95f;

	this->mSwitches.push_back(ps2);
	this->mSwitches.at(index)->Initialize(switchPos, 1, 3, rot, XMFLOAT3(0, -5.0f, 5.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityX(XM_PI);
	index++;

	///////////////////////////////////////////////////////

	//ok
	PlatformSwitch* ps3 = new PlatformSwitch();
	switchPos = pos;
	switchPos.x += -129.957f;
	switchPos.y += 21.513f;
	switchPos.z += 91.85f;

	this->mSwitches.push_back(ps3);
	this->mSwitches.at(index)->Initialize(switchPos, 3, 10, rot, XMFLOAT3(-5.0f, 0.0f, 5.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityY(XM_PI/2);
	index++;

	////////////////////////////////////////////////////

	//ok
	PlatformSwitch* ps4 = new PlatformSwitch();
	switchPos = pos;
	switchPos.x += 53.692f;
	switchPos.y += -3.895f;
	switchPos.z += 91.876f;

	this->mSwitches.push_back(ps4);
	this->mSwitches.at(index)->Initialize(switchPos, 3, 9, rot, XMFLOAT3(5.0f, 0.0f, 5.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityY(XM_PI);
	index++;

	//////////////////////////////////////////////////////

	//ok
	PlatformSwitch* ps5 = new PlatformSwitch();
	switchPos = pos;
	switchPos.x += -54.435f;
	switchPos.y += 48.159f;
	switchPos.z += -91.851f;

	this->mSwitches.push_back(ps5);
	this->mSwitches.at(index)->Initialize(switchPos, 1, 1, rot, XMFLOAT3(0, 0.0f, 0.0f));
	index++;

	//////////////////////////////////////////////////////////////////

	//ok
	PlatformSwitch* ps6 = new PlatformSwitch();
	switchPos = pos;
	switchPos.x += 53.832f;
	switchPos.y += 48.159f;
	switchPos.z += -8.432f;

	this->mSwitches.push_back(ps6);
	this->mSwitches.at(index)->Initialize(switchPos, 2, 5, rot, XMFLOAT3(0, 0.0f, 0.0f));
	index++;
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
