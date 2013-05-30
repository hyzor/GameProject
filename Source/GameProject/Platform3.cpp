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

	int index = 0;

	XMFLOAT4X4 rot;

	XMStoreFloat4x4(&rot, XMMatrixIdentity());
	PlatformSwitch* ps = new PlatformSwitch();
	this->mSwitches.push_back(ps);
	this->mSwitches.at(index)->Initialize(pos, XMFLOAT3(-31.529f, -28.942f, 79.934f), 1, 3, rot, XMFLOAT3(0, -5.0f, 5.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityX(XM_PI);
	index++;

	/////////////////////////////////////////////////////////////////////

	PlatformSwitch* ps1 = new PlatformSwitch();
	this->mSwitches.push_back(ps1);
	this->mSwitches.at(index)->Initialize(pos, XMFLOAT3(-79.77f, 7.137f, 79.965f), 3, 10, rot, XMFLOAT3(-5.0f, 0.0f, 5.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityY(XM_PI/2);
	index++;

	/////////////////////////////////////////////////////////7

	PlatformSwitch* ps2 = new PlatformSwitch();
	this->mSwitches.push_back(ps2);
	this->mSwitches.at(index)->Initialize(pos, XMFLOAT3(-79.961f, 28.954f, 14.319f), 2, 6, rot, XMFLOAT3(-5.0f, 0.0f, 0.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityZ(XM_PI/2);
	index++;

	///////////////////////////////////////////////////////

	//ok
	PlatformSwitch* ps3 = new PlatformSwitch();
	this->mSwitches.push_back(ps3);
	this->mSwitches.at(index)->Initialize(pos, XMFLOAT3(-41.709f, 28.957f, -79.941f), 1, 1, rot, XMFLOAT3(0.0f, 0.0f, 0.0f));
	index++;

	////////////////////////////////////////////////////

	//ok
	PlatformSwitch* ps4 = new PlatformSwitch();
	this->mSwitches.push_back(ps4);
	this->mSwitches.at(index)->Initialize(pos, XMFLOAT3(79.789f, 2.715f, -79.941f), 3, 11, rot, XMFLOAT3(5.0f, 0.0f, -5.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityY(-XM_PI/2);
	index++;

	//////////////////////////////////////////////////////

	//ok
	PlatformSwitch* ps5 = new PlatformSwitch();
	this->mSwitches.push_back(ps5);
	this->mSwitches.at(index)->Initialize(pos, XMFLOAT3(-79.772f, -29.101f, -19.187f), 2, 7, rot, XMFLOAT3(-5.0f, -5.0f, 0.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityZ(XM_PI);
	index++;

	//////////////////////////////////////////////////////////////////

	//ok
	PlatformSwitch* ps6 = new PlatformSwitch();
	this->mSwitches.push_back(ps6);
	this->mSwitches.at(index)->Initialize(pos, XMFLOAT3(11.727f, 28.957f, 79.934f), 1, 2, rot, XMFLOAT3(0, 5.0f, 10.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityY(XM_PI);
	index++;
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
