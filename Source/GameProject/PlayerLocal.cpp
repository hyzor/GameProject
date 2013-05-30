#include "PlayerLocal.h"
#include <math.h>
#include "GUI.h"

PlayerLocal::PlayerLocal(std::string Nickname, XMFLOAT3 Position) : PlayerLocal::Player(0, Nickname, Position, 0)
{
	//send connect package
	char* name = new char[50];
	for(UINT i = 0; i < Nickname.length(); i++)
		name[i] = Nickname[i];
	Network::GetInstance()->Push(new Package(Package::Header(2, 0, 50), Package::Body(name)));

	t = 0;
	rotateTo = XMFLOAT3(0,0,0);
	TimeToSpawn = 0;
}

PlayerLocal::~PlayerLocal()
{
}

void PlayerLocal::Update(float dt, float gameTime, DirectInput* dInput, SoundModule* sm, World* world, std::vector<Player*>* multiplayers)
{
	XMVECTOR pos = XMLoadFloat3(&mPosition);
	XMMATRIX cJoint = XMLoadFloat4x4(&Joint);

	//send package
	t += dt;
	if(t > 0.1f)
	{
		t = 0;

		struct Data
		{
			XMFLOAT3 pos;
			XMFLOAT3 mov;
			XMFLOAT3 rot;
			float pitch;
			float roll;
			float yaw;
		};

		Data* data = new Data();
		data->pos = this->mPosition;
		data->mov = this->move;
		data->rot = this->rotation;
		data->pitch = this->mCamera->Pitch;
		data->roll = this->mCamera->Roll;
		data->yaw = this->mCamera->Yaw;

		Network::GetInstance()->Push(new Package(Package::Header(1, 1, sizeof(Data)), Package::Body((char*)(data))));
	}


	
	XMVECTOR m = XMLoadFloat3(&XMFLOAT3(0,0,0));

	if(!rotating && mIsAlive)
	{
		//gravity switch
		XMVECTOR dir = XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,-1,0)), cJoint);

		PlatformSwitch* psCol = world->IntersectSwitch(pos+XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,10,0)), cJoint), dir, 10);
		if(psCol != NULL)
		{
			XMVECTOR up = XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,1,0)), cJoint);
			SwitchRotations sr = psCol->GetRotations(up);

			if(!(sr.rot.x == 0 && sr.rot.y == 0 && sr.rot.z == 0))
			{
				rotation = sr.start;
				rotateTo = sr.start;
				rotateTo.x += sr.rot.x;
				rotateTo.y += sr.rot.y;
				rotateTo.z += sr.rot.z;

				ySpeed = 0;
				rotating = true;
				pos = psCol->GetMoveTo(up);

				cJoint = XMMatrixRotationX(rotation.x) * XMMatrixRotationY(rotation.y) * XMMatrixRotationZ(rotation.z);
			}
		}

		//switch garvity
		//switch garvity
		if(dInput->GetKeyboardState()[DIK_E] & 0x80)
		{
			if(!eDown)
			{
				rotateTo.x += (float)PI/2;
				ySpeed = 0;
				rotating = true;
			}
			eDown = true;
		}
		else
			eDown = false;
		if(dInput->GetKeyboardState()[DIK_R] & 0x80)
		{
			if(!rDown)
			{
				rotateTo.z -= (float)PI/2;
				ySpeed = 0;
				rotating = true;
			}
			rDown = true;
		}
		else
			rDown = false;
		if(dInput->GetKeyboardState()[DIK_T] & 0x80)
		{
			if(!tDown)
			{
				rotateTo.y += (float)PI/2;
				ySpeed = 0;
				rotating = true;
			}
			tDown = true;
		}
		else
			tDown = false;

		//Remove down movement
		XMVECTOR removeDown = XMLoadFloat3(&XMFLOAT3(0,1,0));
		removeDown = XMVector3Transform(removeDown, cJoint);
		if(std::abs(XMVectorGetX(removeDown)) < 0.5f)
			removeDown = XMVectorSetX(removeDown, 1);
		else
			removeDown = XMVectorSetX(removeDown, 0);
		if(std::abs(XMVectorGetY(removeDown)) < 0.5f)
			removeDown = XMVectorSetY(removeDown, 1);
		else
			removeDown = XMVectorSetY(removeDown, 0);
		if(std::abs(XMVectorGetZ(removeDown)) < 0.5f)
			removeDown = XMVectorSetZ(removeDown, 1);
		else
			removeDown = XMVectorSetZ(removeDown, 0);
	
		//move
		XMVECTOR look = XMVector3Normalize(mCamera->GetLookXM()*removeDown);
		if (dInput->GetKeyboardState()[DIK_W] & 0x80)
			m += look*mSpeed*dt;
		if (dInput->GetKeyboardState()[DIK_S] & 0x80)
			m -= look*mSpeed*dt;
		XMVECTOR right = XMVector3Normalize(mCamera->GetRightXM()*removeDown);
		if (dInput->GetKeyboardState()[DIK_D] & 0x80)
			m += right*mSpeed*dt;
		if (dInput->GetKeyboardState()[DIK_A] & 0x80)
			m -= right*mSpeed*dt;

		// Rotate camera
		if (dInput->MouseHasMoved())
		{
			float dx = XMConvertToRadians(0.25f*static_cast<float>(dInput->GetMouseState().lX));
			float dy = XMConvertToRadians(0.25f*static_cast<float>(dInput->GetMouseState().lY));

			mCamera->Yaw += dx;;
			mCamera->Pitch += dy;
		}

		//jump
		if (this->OnGround && dInput->GetKeyboardState()[DIK_SPACE] & 0x80)
			ySpeed = -200;

		// Shoot
		if (dInput->GetMouseState().rgbButtons[0])
		{
			XMVECTOR v = XMLoadFloat3(&mPosition)+XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,15,0)), cJoint);
			XMFLOAT3 p;
			XMStoreFloat3(&p,v);

			XMVECTOR dir = XMVector3Normalize(mCamera->GetLookXM());
			//platform test
			World::Hit h = world->Intersect(v, dir, 100000);
			int hitId = 0;
			//enemy test
			for(UINT i = 0; i < multiplayers->size(); i++)
			{
				if(multiplayers->at(i)->IsAlive())
				{
					float t = 0;
					if(XNA::IntersectRayAxisAlignedBox(v,dir,&multiplayers->at(i)->GetBounding(), &t))
						if(t < h.hit.t)
						{
							h.hit.t = t;
							h.hit.hit = true;
							hitId = multiplayers->at(i)->GetID();
						}
				}
			}

			//fire
			if (mWeapons.at(mCurWeaponIndex)->FireProjectile(p, mCamera->GetLook()))
			{
				sm->playSFX(p, FireWeapon, false);

				struct Data
				{
					int hitId;
					XMFLOAT3 hitAt;
				};

				Data* data = new Data();
				XMStoreFloat3(&data->hitAt, v+dir*h.hit.t);
				data->hitId = hitId;

				Network::GetInstance()->Push(new Package(Package::Header(11, 1, sizeof(Data)), Package::Body((char*)(data))));
			}
		}
	}
	else
	{
		//rotating animation
		if(rotateTo.x != rotation.x)
		{
			float rt = rotateTo.x;
			float r = rotation.x;

			if(rt > (float)PI)
				rt = -((float)PI*2-rt);
			if(r > (float)PI)
				r = -((float)PI*2-r);

			if(rotateTo.x > rotation.x)
			{
				rotation.x += 5*dt;
				r += 5*dt;
				if(rt < r)
				{
					rotation.x = rotateTo.x;
					rotating = false;
				}
			}
			else
			{
				rotation.x -= 5*dt;
				r -= 5*dt;
				if(rt > r)
				{
					rotation.x = rotateTo.x;
					rotating = false;
				}
			}
		}
		else if(rotateTo.y != rotation.y)
		{
			float rt = rotateTo.y;
			float r = rotation.y;

			if(rt > (float)PI)
				rt = -((float)PI*2-rt);
			if(r > (float)PI)
				r = -((float)PI*2-r);

			if(rotateTo.y > rotation.y)
			{
				rotation.y += 5*dt;
				r += 5*dt;
				if(rt < r)
				{
					rotation.y = rotateTo.y;
					rotating = false;
				}
			}
			else
			{
				rotation.y -= 5*dt;
				r -= 5*dt;
				if(rt > r)
				{
					rotation.y = rotateTo.y;
					rotating = false;
				}
			}
		}
		else if(rotateTo.z != rotation.z)
		{
			float rt = rotateTo.z;
			float r = rotation.z;

			if(rt > (float)PI)
				rt = -((float)PI*2-rt);
			if(r > (float)PI)
				r = -((float)PI*2-r);

			if(rotateTo.z > rotation.z)
			{
				rotation.z += 5*dt;
				r += 5*dt;
				if(rt < r)
				{
					rotation.z = rotateTo.z;
					rotating = false;
				}
			}
			else
			{
				rotation.z -= 5*dt;
				r -= 5*dt;
				if(rt > r)
				{
					rotation.z = rotateTo.z;
					rotating = false;
				}
			}
		}
		else
			rotating = false;
	}

	
	/*if(!mIsAlive) //flytta till servern
	{
		std::vector<int> iReturns(0);
		Python->LoadModule("respawn");
		Python->CallFunction(
			Python->GetFunction("CheckSpawnTimer"),
			nullptr);
		Python->Update(0.0f);
		if(Python->CheckReturns())
		{
			Python->ConvertInts(iReturns);
			Python->ClearReturns();
			TimeToSpawn = iReturns[0];
		}
		if(TimeToSpawn <= 0)
		{
			Spawn(1,300,50, 0);
		}
	}*/

	XMStoreFloat4x4(&this->Joint, cJoint);
	XMStoreFloat3(&move, m);
	XMStoreFloat3(&mPosition, pos);

	this->Player::Update(dt, gameTime, dInput, sm, world, multiplayers);

}

void PlayerLocal::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap)
{

	if(!mIsAlive)
	{
		wstringstream wss;
		wss << TimeToSpawn;
		Gui->drawText(dc, (wchar_t*)wss.str().c_str(), XMFLOAT2(SCREEN_WIDTH/2-50.0f,SCREEN_HEIGHT/2-50.0f), 70, 0xff0000ff);
	}

	activeTech = Effects::NormalMapFX->DirLights3TexAlphaClipTech;
	mWeapons[mCurWeaponIndex]->Draw(dc, activeTech, mCamera, shadowMap);

	this->Player::Draw(dc, activeTech, mCamera, shadowMap);
}
	
void PlayerLocal::HandelPackage(Package *p)
{
	if (p->GetHeader().operation == 10)
	{
		Package::Body b = p->GetBody();
		this->mIsAlive = (*(int*)b.Read(4))==1;
		this->mHealth = *(float*)b.Read(4);
		this->kills = *(int*)b.Read(4);
		this->deaths = *(int*)b.Read(4);
		this->respawntime = *(float*)b.Read(4);
	}

	Player::HandelPackage(p);
}

void PlayerLocal::Spawn(float x, float y, float z, int rotation) //flytta till servern
{
	std::vector<double> dReturns(0);
	std::vector<int> iReturns(0);
	XMFLOAT3 pos;
	Python->LoadModule("respawn");
	Python->CallFunction(
		Python->GetFunction("getSpawnPos"),
		nullptr);

		Python->Update(0.0f);

		if(Python->CheckReturns())
		{
			Python->ConvertDoubles(dReturns);
			Python->ConvertInts(iReturns);
			Python->ClearReturns();
			
			pos.x = (float)dReturns[0];
			pos.y = (float)dReturns[1];
			pos.z = (float)dReturns[2];

			rotation = iReturns[0];
		}
	
	this->mPosition = pos;
	//this->mIsAlive = true;
	XMStoreFloat4x4(&this->Joint, XMMatrixIdentity());
}