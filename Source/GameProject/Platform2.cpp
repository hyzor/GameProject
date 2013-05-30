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

	XMFLOAT4X4 rot;

	XMStoreFloat4x4(&rot, XMMatrixIdentity());
	PlatformSwitch* ps = new PlatformSwitch();
	this->mSwitches.push_back(ps);
	this->mSwitches.at(index)->Initialize(pos, XMFLOAT3(-129.8f, -0.95f, -91.9f), 3, 8, rot, XMFLOAT3(0, 0, 0));
	index++;

	/////////////////////////////////////////////////////////////////////

	PlatformSwitch* ps1 = new PlatformSwitch();
	this->mSwitches.push_back(ps1);
	this->mSwitches.at(index)->Initialize(pos, XMFLOAT3(8.64f, -48.578f, -91.851f), 1, 4, rot, XMFLOAT3(0, -5.0f, 0.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityX(-XM_PI/2);
	index++;

	/////////////////////////////////////////////////////////7

	PlatformSwitch* ps2 = new PlatformSwitch();
	this->mSwitches.push_back(ps2);
	this->mSwitches.at(index)->Initialize(pos, XMFLOAT3(-33.338f, -48.542f, 91.95f), 1, 3, rot, XMFLOAT3(0, -5.0f, 5.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityX(XM_PI);
	index++;

	///////////////////////////////////////////////////////

	//ok
	PlatformSwitch* ps3 = new PlatformSwitch();
	this->mSwitches.push_back(ps3);
	this->mSwitches.at(index)->Initialize(pos, XMFLOAT3(-129.957f, 21.513f, 91.85f), 3, 10, rot, XMFLOAT3(-5.0f, 0.0f, 5.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityY(XM_PI/2);
	index++;

	////////////////////////////////////////////////////

	//ok
	PlatformSwitch* ps4 = new PlatformSwitch();
	this->mSwitches.push_back(ps4);
	this->mSwitches.at(index)->Initialize(pos, XMFLOAT3(53.692f, -3.895f, 91.876f), 3, 9, rot, XMFLOAT3(5.0f, 0.0f, 5.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityY(XM_PI);
	index++;

	//////////////////////////////////////////////////////

	//ok
	PlatformSwitch* ps5 = new PlatformSwitch();
	this->mSwitches.push_back(ps5);
	this->mSwitches.at(index)->Initialize(pos, XMFLOAT3(-54.435f, 48.159f, -91.851f), 1, 1, rot, XMFLOAT3(0, 0.0f, 0.0f));
	index++;

	//////////////////////////////////////////////////////////////////

	//ok
	PlatformSwitch* ps6 = new PlatformSwitch();
	this->mSwitches.push_back(ps6);
	this->mSwitches.at(index)->Initialize(pos, XMFLOAT3(53.832f, 48.159f, -8.432f), 2, 5, rot, XMFLOAT3(0, 0.0f, 0.0f));
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
