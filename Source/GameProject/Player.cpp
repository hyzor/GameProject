#include "Player.h"

Player::Player(int PlayerID, std::string Nickname, XMFLOAT3 Position)
	: mPlayerID(PlayerID), mNickname(Nickname), mPosition(Position)
{
	Python->LoadModule("scoreboard_script");
	Python->CallFunction(
		Python->GetFunction("CreatePlayerStats"),
		Python->CreateArg(PlayerID, Nickname.c_str()));

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
	aliveTime = 0;

	this->Joint = new XMMATRIX(XMMatrixIdentity());
	
	rotation = XMFLOAT3(0,0,0);
	mCamera = new Camera();


	SetPosition(mPosition);
}


Player::~Player()
{
	SafeDelete(mCamera);
	SafeDelete(Joint);

	for (UINT i = 0; i < mWeapons.size(); ++i)
		delete mWeapons[i];
}

bool Player::Shoot()
{
	XMMATRIX cJoint = *Joint;
	XMVECTOR v = XMLoadFloat3(&mPosition)+XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,15,0)), cJoint);
	XMFLOAT3 p;
	XMStoreFloat3(&p,v);
	if (mWeapons.at(mCurWeaponIndex)->FireProjectile(p, mCamera->GetLook()))
		return true;

	return false;
}


void Player::Kill()
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
}

void Player::Update(float dt, float gameTime, DirectInput* dInput, SoundModule* sm, World* world)
{
	XMMATRIX cJoint = *Joint;
	XMVECTOR pos = XMLoadFloat3(&mPosition);	

	// Health lower than or equal 0, die
	if (mHealth <= 0.0f)
		Die();

	// Move
	pos += XMLoadFloat3(&move);	


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

	dir = XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,-1,0)), cJoint);
	hit = world->Intersect(pos+XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,30,0)), cJoint), dir, 30); 
	if(hit.hit)
	{
		//feet
		if(hit.t > 15)
		{
			OnGround = true;
			ySpeed = 0;
			pos -= dir*30-dir*hit.t;
		}
		//head
		if(hit.t < 15)
		{
			OnGround = false;
			ySpeed = 0;
			pos += dir*hit.t;
		}
	}

	dir = XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,0,1)), cJoint);
	hit = world->Intersect(pos+XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,2.5f,-3)), cJoint), dir, 6); 
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
	hit = world->Intersect(pos+XMVector3Transform(XMLoadFloat3(&XMFLOAT3(-3,2.5f,0)), cJoint), dir, 6); 
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
		sm->playSFX(mPosition, Running, false);
	else
		sm->stopSound(Running);		


	cJoint = XMMatrixRotationX(rotation.x) * XMMatrixRotationY(rotation.y) * XMMatrixRotationZ(rotation.z);//XMMatrixLookAtLH(XMLoadFloat3(&XMFLOAT3(0,0,0)), XMLoadFloat3(&XMFLOAT3(1,0,0)), XMLoadFloat3(&XMFLOAT3(0,1,0)));

	XMStoreFloat3(&mPosition, pos);

	XMFLOAT3 camPos;
	XMStoreFloat3(&camPos, pos+XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,20,0)), cJoint));
	mCamera->SetPosition(camPos);
	mCamera->UpdateViewMatrix(XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0, 1, 0)), cJoint), XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0, 0, 1)), cJoint), XMVector3Transform(XMLoadFloat3(&XMFLOAT3(1, 0, 0)), cJoint));

	// Update player weapons
	mWeapons[mCurWeaponIndex]->Update(dt, gameTime);
	mWeapons[mCurWeaponIndex]->ViewMatrixRotation(mCamera->GetViewMatrix());

	delete Joint;
	this->Joint = new XMMATRIX(cJoint);

	aliveTime += dt;
}

void Player::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap)
{
	activeTech = Effects::NormalMapFX->DirLights3TexAlphaClipTech;
	mWeapons[mCurWeaponIndex]->Draw(dc, activeTech, mCamera, shadowMap);
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

bool Player::OutOfMap() //flytta till server
{
	if( mPosition.x > 1000  ||
		mPosition.x < -1000 ||
		mPosition.y > 1000  ||
		mPosition.y < -1000 ||
		mPosition.z > 1000  ||
		mPosition.z < -1000)
	{
			return true;
	}

	return false;

}