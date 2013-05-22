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

	//------------------------------------
	// Create lights
	//------------------------------------
	PointLight pLight1;
	pLight1.Position = pos;
	pLight1.Position.x += -163.302f;
	pLight1.Position.y += -118.477f;
	pLight1.Position.z += -36.601f;

	PointLight pLight2;
	pLight2.Position = pos;
	pLight2.Position.x += 144.096f;
	pLight2.Position.y += 102.614f;
	pLight2.Position.z += -66.06f;

	mPointLights.push_back(pLight1);
	mPointLights.push_back(pLight2);

	for (UINT i = 0; i < mPointLights.size(); ++i)
	{
		mPointLights[i].Ambient		= XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
		mPointLights[i].Diffuse		= XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
		mPointLights[i].Specular	= XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
		mPointLights[i].Attenuation = XMFLOAT3(0.0f, 0.1f, 0.0f);
		mPointLights[i].Range		= 50.0f;
	}

	int index = 0;

	XMFLOAT3 switchPos = pos;
	/*switchPos.y += 100.0;
	switchPos.x += 50.0f;
	switchPos.z += 72.5f;*/
	switchPos.x += -47.3f;
	switchPos.y += 16.8f;
	switchPos.z += -78.8f;

	//ok
	XMFLOAT4X4 rot;
	XMStoreFloat4x4(&rot, XMMatrixIdentity());
	PlatformSwitch* ps = new PlatformSwitch();
	this->mSwitches.push_back(ps);
	this->mSwitches.at(index)->Initialize(switchPos, 1, 1, rot, XMFLOAT3(0, 0, 0));
	index++;
	//this->mSwitches.at(0)->getEntity()->RotateEntityX(XM_PI/2);

	//ok
	PlatformSwitch* ps1 = new PlatformSwitch();
	switchPos = pos;
	switchPos.x += -87.9f;
	switchPos.y += 100.4f;
	switchPos.z += 70.8f;

	
	XMStoreFloat4x4(&rot, XMMatrixRotationY(XM_PI));
	this->mSwitches.push_back(ps1);
	this->mSwitches.at(index)->Initialize(switchPos, 1, 2, rot, XMFLOAT3(0, 5.0f, 5.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityY(XM_PI);
	index++;

	////ok
	PlatformSwitch* ps2 = new PlatformSwitch();
	switchPos = pos;
	switchPos.x += -28.4f;
	switchPos.y += -31.5f;
	switchPos.z += 70.8f;

	this->mSwitches.push_back(ps2);
	this->mSwitches.at(index)->Initialize(switchPos, 1, 3, rot, XMFLOAT3(0, -5.0f, 5.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityX(XM_PI);
	index++;

	////ok
	/*PlatformSwitch* ps3 = new PlatformSwitch();
	switchPos = pos;
	switchPos.x += 178.3f;
	switchPos.y += 100.4f;
	switchPos.z += 48.5f;

	this->mSwitches.push_back(ps3);
	this->mSwitches.at(index)->Initialize(switchPos, 1, 3, rot, XMFLOAT3(0, -5.0f, 5.0f));
	this->mSwitches.at(index)->getEntity()->RotateEntityY(-XM_PI/2);
	index++;*/

	////ok, tack vare fullösning
	//PlatformSwitch* ps4 = new PlatformSwitch();
	//switchPos = pos;
	//switchPos.x += 178.3f;
	//switchPos.y += 5.8f;
	//switchPos.z += 70.6f;

	//this->mSwitches.push_back(ps4);
	//this->mSwitches.at(index)->Initialize(switchPos, 3);
	//this->mSwitches.at(index)->getEntity()->RotateEntityY(XM_PI);
	//index++;

	//ok
	
	PlatformSwitch* ps5 = new PlatformSwitch();
	switchPos = pos;
	switchPos.x += 81.7f;
	switchPos.y += -31.6f;
	switchPos.z += -78.6f;

	XMStoreFloat4x4(&rot, XMMatrixRotationX(-XM_PI/2));
	this->mSwitches.push_back(ps5);
	this->mSwitches.at(index)->Initialize(switchPos, 1, 4, rot, XMFLOAT3(0, -5.0f, 0.0f)/*XMFLOAT3(0, 0.0f, 0.0f)*/);
	this->mSwitches.at(index)->getEntity()->RotateEntityX(-XM_PI/2);
	index++;
	//Platform::Initialize(id, pos);

	//int index = 0;

	//XMFLOAT3 switchPos = pos;
	///*switchPos.y += 100.0;
	//switchPos.x += 50.0f;
	//switchPos.z += 72.5f;*/
	//switchPos.y += 16.8f;
	//switchPos.x += -47.3f;
	//switchPos.z += -78.8f;

	////ok
	//PlatformSwitch* ps = new PlatformSwitch();
	//this->mSwitches.push_back(ps);
	//this->mSwitches.at(index)->Initialize(switchPos);
	//index++;
	////this->mSwitches.at(0)->getEntity()->RotateEntityX(XM_PI/2);

	////ok
	//PlatformSwitch* ps1 = new PlatformSwitch();
	//switchPos = pos;
	//switchPos.x += -87.9f;
	//switchPos.y += 100.4f;
	//switchPos.z += 70.8f;

	//this->mSwitches.push_back(ps1);
	//this->mSwitches.at(index)->Initialize(switchPos);
	//this->mSwitches.at(index)->getEntity()->RotateEntityY(XM_PI);
	//index++;

	////ok
	//PlatformSwitch* ps2 = new PlatformSwitch();
	//switchPos = pos;
	//switchPos.x += -28.4f;
	//switchPos.y += -31.5f;
	//switchPos.z += 70.8f;

	//this->mSwitches.push_back(ps2);
	//this->mSwitches.at(index)->Initialize(switchPos);
	//this->mSwitches.at(index)->getEntity()->RotateEntityX(XM_PI);
	//index++;

	////ok
	//PlatformSwitch* ps3 = new PlatformSwitch();
	//switchPos = pos;
	//switchPos.x += 178.3f;
	//switchPos.y += 100.4f;
	//switchPos.z += 48.5f;

	//this->mSwitches.push_back(ps3);
	//this->mSwitches.at(index)->Initialize(switchPos);
	//this->mSwitches.at(index)->getEntity()->RotateEntityY(-XM_PI/2);
	//index++;

	////inte ok
	////PlatformSwitch* ps4 = new PlatformSwitch();
	////switchPos = pos;
	////switchPos.x += 178.3f;
	////switchPos.y += 5.8f;
	////switchPos.z += 70.6f;

	////this->mSwitches.push_back(ps4);
	////this->mSwitches.at(index)->Initialize(switchPos);
	////index++;

	////ok
	//PlatformSwitch* ps5 = new PlatformSwitch();
	//switchPos = pos;
	//switchPos.x += 81.7f;
	//switchPos.y += -31.6f;
	//switchPos.z += -78.6f;

	//this->mSwitches.push_back(ps5);
	//this->mSwitches.at(index)->Initialize(switchPos);
	//this->mSwitches.at(index)->getEntity()->RotateEntityX(-XM_PI/2);
	//index++;
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
