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

	//------------------------------------
	// Create lights
	//------------------------------------
	PointLight pLight1;
	pLight1.Position = pos;
	pLight1.Position.x += 27.227;
	pLight1.Position.y += 79.323f;
	pLight1.Position.z += 97.226f;

	mPointLights.push_back(pLight1);

	mPointLightOffsets.push_back(XMFLOAT3(27.227, 79.323f, 97.226f));

	for (UINT i = 0; i < mPointLights.size(); ++i)
	{
		mPointLights[i].Ambient		= XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
		mPointLights[i].Diffuse		= XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
		mPointLights[i].Specular	= XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
		mPointLights[i].Attenuation = XMFLOAT3(0.0f, 0.1f, 0.0f);
		mPointLights[i].Range		= 100.0f;
	}

	int index = 0;
	XMFLOAT4X4 rot;

	XMStoreFloat4x4(&rot, XMMatrixIdentity());
	PlatformSwitch* ps = new PlatformSwitch();
	this->mSwitches.push_back(ps);
	this->mSwitches.at(index)->Initialize(pos, XMFLOAT3(44.448f, -51.658f, -108.818f), 3, 11, rot, XMFLOAT3(5.0f, 0.0f, -5.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityY(-XM_PI/2);
	index++;

	/////////////////////////////////////////////////////////////////////

	PlatformSwitch* ps1 = new PlatformSwitch();
	this->mSwitches.push_back(ps1);
	this->mSwitches.at(index)->Initialize(pos, XMFLOAT3(44.442f, -78.477f, -16.483f), 2, 12, rot, XMFLOAT3(5.0f, -5.0f, 0));
	this->mSwitches.at(index)->getEntity()->RotateEntityZ(-XM_PI/2);
	index++;

	/////////////////////////////////////////////////////////7

	PlatformSwitch* ps2 = new PlatformSwitch();
	this->mSwitches.push_back(ps2);
	this->mSwitches.at(index)->Initialize(pos, XMFLOAT3(3.383f, -78.315f, 108.886f), 1, 3, rot, XMFLOAT3(0, -10.0f, 5.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityX(XM_PI);
	index++;

	///////////////////////////////////////////////////////

	//ok
	PlatformSwitch* ps3 = new PlatformSwitch();
	this->mSwitches.push_back(ps3);
	this->mSwitches.at(index)->Initialize(pos, XMFLOAT3(-44.568f, -44.145f, 108.701f), 3, 10, rot, XMFLOAT3(-5.0f, 0.0f, 5.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityY(XM_PI/2);
	index++;

	////////////////////////////////////////////////////

	//ok
	PlatformSwitch* ps4 = new PlatformSwitch();
	this->mSwitches.push_back(ps4);
	this->mSwitches.at(index)->Initialize(pos, XMFLOAT3(-32.571f, -24.59f, -30.935f), 2, 6, rot, XMFLOAT3(-5.0f, 0.0f, 0.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityZ(XM_PI/2);
	index++;

	//////////////////////////////////////////////////////

	//ok
	PlatformSwitch* ps5 = new PlatformSwitch();
	this->mSwitches.push_back(ps5);
	this->mSwitches.at(index)->Initialize(pos, XMFLOAT3(-12.918f, -12.54f, -108.889f), 1, 1, rot, XMFLOAT3(0, 0, 0));
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
