#include "Player.h"

Player::Player(int PlayerID, std::string Nickname, XMFLOAT3 Position)
	: mPlayerID(PlayerID), mNickname(Nickname), mPosition(Position)
{
	Python->LoadModule("scoreboard_script");
	Python->CallFunction(
		Python->GetFunction("CreatePlayerStats"),
		Python->CreateArg(PlayerID, Nickname.c_str()));

	mHealth = 1;
	mNickname = Nickname;
	mSpeed = 100;
	ySpeed = 0;
	mPosition = Position;
	mPlayerID = PlayerID;
	mIsAlive = true;
	OnGround = false;
	rotating = false;
	move = XMFLOAT3(0, 0, 0);

	this->Joint = new XMMATRIX(XMMatrixIdentity());

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
	if (mWeapons.at(mCurWeaponIndex)->FireProjectile(mPosition, mCamera->GetLook()))
		return true;

	return false;
}

void Player::TakeDamage(float damage)
{
	mHealth -= damage;
}

void Player::Update(float dt, float gameTime, DirectInput* dInput, SoundModule* sm, World* world)
{
	XMMATRIX cJoint = *Joint;
	XMVECTOR pos = XMLoadFloat3(&mPosition);	

	// Health lower than 0, die
	if (mHealth < 0.0f)
		mIsAlive = false;

	// Update player weapons
	mWeapons[mCurWeaponIndex]->Update(dt, gameTime);

	// Move
	pos += XMLoadFloat3(&move);	

	// Shoot
	if (dInput->GetMouseState().rgbButtons[0])
	{
		if (Shoot())
			//int temp = 1;
			sm->playSFX(mPosition, FireWeapon, false);
	}

	// Gravity
	if(!rotating)
	{
		ySpeed += 300*dt;
		pos -= XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,1,0))*ySpeed*dt, cJoint);
	}

	//Collision
	this->OnGround = false;
	XMVECTOR dir;
	CollisionModel::Hit hit;

	dir = XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,-1,0)), cJoint);
	hit = world->Intersect(pos+XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,20,0)), cJoint), dir, 20); 
	if(hit.hit)
	{
		//feet
		if(hit.t > 10)
		{
			OnGround = true;
			ySpeed = 0;
			pos -= dir*20-dir*hit.t;
		}
		//head
		if(hit.t < 10)
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
	
	XMStoreFloat3(&mPosition, pos);

	XMFLOAT3 camPos;
	XMStoreFloat3(&camPos, pos+XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,10,0)), cJoint));
	mCamera->SetPosition(camPos);
	mCamera->UpdateViewMatrix(XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0, 1, 0)), cJoint), XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0, 0, 1)), cJoint), XMVector3Transform(XMLoadFloat3(&XMFLOAT3(1, 0, 0)), cJoint));

	delete Joint;
	this->Joint = new XMMATRIX(cJoint);
}

void Player::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap)
{
	mWeapons[mCurWeaponIndex]->Draw(dc, mCamera);
}

void Player::HandelPackage(Package *p)
{
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
	railgun->Init(prop, device, dc);

	mWeapons.push_back(railgun);
	this->mCurWeaponIndex = 0;
}