#include "Player.h"

Player::Player(SoundModule* sm, int PlayerID, std::string Nickname, XMFLOAT3 Position, int index)
	: mPlayerID(PlayerID), mNickname(Nickname), mPosition(Position)
{
	Python->LoadModule("scoreboard_script");
	Python->CallFunction(
		Python->GetFunction("CreatePlayerStats"),
		Python->CreateArg(PlayerID, Nickname.c_str()));

	this->sm = sm;

	mHealth = 0;
	mNickname = Nickname;
	mSpeed = 100;
	ySpeed = 0;
	mPosition = Position;
	mPlayerID = PlayerID;
	mIsAlive = false;
	OnGround = false;
	rotating = false;
	move = XMFLOAT3(0, 0, 0);
	relativeMotion = XMFLOAT3(0,0,0);
	aliveTime = 0;
	kills = 0;
	deaths = 0;
	this->index = index;

	XMStoreFloat4x4(&this->Joint, XMMatrixIdentity());
	
	rotation = XMFLOAT3(0,0,0);
	mCamera = new Camera();


	SetPosition(mPosition);
}


Player::~Player()
{
	SafeDelete(mCamera);

	for (UINT i = 0; i < mWeapons.size(); ++i)
		delete mWeapons[i];
}


/*void Player::Kill()
{
	Python->LoadModule("scoreboard_script");
		Python->CallFunction(
		Python->GetFunction("AddKill"),
		Python->CreateArg(this->mPlayerID));
}

void Player::Die()
{
	Python->Update(0.0f);
	Python->LoadModule("respawn");
	Python->CallFunction(
		Python->GetFunction("PlayerDied"),
		NULL);

	Python->LoadModule("scoreboard_script");
		Python->CallFunction(
		Python->GetFunction("AddDeath"),
		Python->CreateArg(this->mPlayerID));

	mIsAlive = false;
}*/

void Player::Update(float dt, float gameTime, DirectInput* dInput, World* world, std::vector<Player*>* multiplayers)
{
	XMMATRIX cJoint = XMLoadFloat4x4(&Joint);
	XMVECTOR pos = XMLoadFloat3(&mPosition);	
	
	// Move
	pos += XMLoadFloat3(&move)+XMLoadFloat3(&relativeMotion)*dt;	


	// Gravity
	if(!rotating)
	{
		if(mIsAlive && aliveTime > 0)
			ySpeed += 300*dt;
		pos -= XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,1,0))*ySpeed*dt, cJoint);
	}

	//Collision
	this->OnGround = false;
	XMVECTOR dir;
	CollisionModel::Hit hit;
	World::Hit groundHit;

	dir = XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,-1,0)), cJoint);
	groundHit = world->Intersect(pos+XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,36,0)), cJoint), dir, 36);
	hit = groundHit.hit;
	if(hit.hit)
	{
		//feet
		if(hit.t > 18)
		{
			OnGround = true;
			ySpeed = 0;
			pos -= dir*36-dir*hit.t;
			relativeMotion = groundHit.platform->move; 
		}
		//head
		if(hit.t < 18)
		{
			OnGround = false;
			ySpeed = 0;
			pos += dir*hit.t;
		}
	}
	

	dir = XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,0,1)), cJoint);
	hit = world->Intersect(pos+XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,2.5f,-3)), cJoint), dir, 6).hit; 
	if(hit.hit)
	{
		//front
		if(hit.t > 3)
			pos -= dir*6-dir*hit.t;
		//back
		if(hit.t < 3)
			pos += dir*hit.t;
	}

	dir = XMVector3Transform(XMLoadFloat3(&XMFLOAT3(-1,0,0)), cJoint);
	hit = world->Intersect(pos+XMVector3Transform(XMLoadFloat3(&XMFLOAT3(-3,2.5f,0)), cJoint), dir, 6).hit; 
	if(hit.hit)
	{
		//left
		if(hit.t > 3)
			pos -= dir*6-dir*hit.t;
		//right
		if(hit.t < 3)
			pos += dir*hit.t;
	}
	
	// Step sound
	if(OnGround && XMVectorGetX(XMVector3Dot(XMLoadFloat3(&move), XMLoadFloat3(&XMFLOAT3(1,1,1)))) != 0)
	{
		if(this->GetID() == 0)
			sm->playSFX(mPosition, Running, false);
		else
			sm->playEnemySFX(Running, this->index, mPosition, false);
	}
	else
	{
		if(this->GetID() == 0)
			sm->stopSound(Running);
		else
			sm->stopEnemySound(Running, this->index);
	}


	cJoint = XMMatrixRotationX(rotation.x) * XMMatrixRotationY(rotation.y) * XMMatrixRotationZ(rotation.z);

	XMStoreFloat3(&mPosition, pos);

	XMFLOAT3 camPos;
	XMStoreFloat3(&camPos, pos+XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,25,0)), cJoint));
	mCamera->SetPosition(camPos);
	mCamera->UpdateViewMatrix(XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0, 1, 0)), cJoint), XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0, 0, 1)), cJoint), XMVector3Transform(XMLoadFloat3(&XMFLOAT3(1, 0, 0)), cJoint));

	// Update player weapons
	mWeapons[mCurWeaponIndex]->Update(dt, gameTime);
	mWeapons[mCurWeaponIndex]->ViewMatrixRotation(mCamera->GetViewMatrix());

	XMStoreFloat4x4(&this->Joint, cJoint);

	aliveTime += dt;
	respawntime = 0;
}

void Player::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap)
{
}

void Player::HandelPackage(Package *p)
{
	if (p->GetHeader().operation == 3)
	{
		Package::Body b = p->GetBody();
		this->mPosition = *(XMFLOAT3*)b.Read(4*3);
		this->mIsAlive = (*(int*)b.Read(4))==1;
		this->mHealth = *(float*)b.Read(4);
		this->aliveTime = 0;
	}
}

void Player::InitWeapons(ID3D11Device* device, ID3D11DeviceContext* dc)
{
	// --- Weapons -------------------------
	// Create railgun
	Railgun* railgun = new Railgun();
	Railgun::Properties prop;
	prop.cooldown = 2.0f;
	prop.damage = 1.0f;
	prop.maxProjectiles = 1;
	railgun->Init(prop, device, dc, "Gun", mPosition);

	mWeapons.push_back(railgun);
	this->mCurWeaponIndex = 0;
}

XNA::AxisAlignedBox Player::GetBounding()
{
	XNA::AxisAlignedBox box;
	XMStoreFloat3(&box.Center, XMLoadFloat3(&mPosition)+XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,10,0)), XMLoadFloat4x4(&Joint)));
	XMStoreFloat3(&box.Extents, XMVector3Transform(XMLoadFloat3(&XMFLOAT3(4,30,4)), XMLoadFloat4x4(&Joint)));

	return box;
}