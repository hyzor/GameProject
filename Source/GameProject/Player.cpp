#include "Player.h"


Player::Player(int PlayerID, std::string Nickname, XMFLOAT3 Position)
{
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

	// Weapons
	mWeapons = std::vector<Weapon*>();
	mWeapons.push_back(new Railgun());
	this->mCurWeaponIndex = 0;
}


Player::~Player()
{
	SafeDelete(mCamera);
	SafeDelete(Joint);

	for (UINT i = 0; i < mWeapons.size(); ++i)
		delete mWeapons[i];
}

void Player::Shoot()
{
	mWeapons.at(mCurWeaponIndex)->FireProjectile();
}

void Player::TakeDamage(float damage)
{
	mHealth -= damage;
}

void Player::Update(float dt, DirectInput* dInput, SoundModule* sm, World* world)
{
	XMMATRIX cJoint = *Joint;
	XMVECTOR pos = XMLoadFloat3(&mPosition);

	

	// Health lower than 0, die
	if (mHealth < 0.0f)
		mIsAlive = false;

	mWeapons[mCurWeaponIndex]->Update(dt);

	// Move
	pos += XMLoadFloat3(&move);

	

	if (dInput->GetMouseState().rgbButtons[0])
		Shoot();




	//gravity
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
}

void Player::HandelPackage(Package *p)
{
}
