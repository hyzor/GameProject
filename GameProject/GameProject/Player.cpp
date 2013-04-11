#include "Player.h"


Player::Player(GenericModel* model, int PlayerID, std::string Nickname, XMFLOAT3 Position)
{

	mInCameraFrustum = true;

	mHealth = 1;
	mNickname = Nickname;
	mSpeed = 100;
	ySpeed = 0;
	mPosition = Position;
	mPlayerID = PlayerID;
	mIsAlive = true;

	mCamera = new Camera();

	mModel = model;

	SetPosition(mPosition);

	// Weapons
	Weapon* weapon = new Weapon();
	Weapon::Properties prop;
	prop.Type = Weapon::TYPE_RAILGUN;
	prop.Damage = 1.0f;
	prop.Cooldown = 5.0f;
	prop.NumProjectiles = 1;
	weapon->Init(prop);
	mWeapons.push_back(weapon);
	mCurWeaponIndex = 0;
}



Player::~Player()
{
	SafeDelete(mCamera);

	for (UINT i = 0; i < mWeapons.size(); ++i)
		delete mWeapons[i];
}

bool Player::Shoot()
{
	if (mWeapons[mCurWeaponIndex]->GetCooldown() > 0.0f)
		return false;

	switch (mWeapons[mCurWeaponIndex]->GetType())
	{
		case Weapon::TYPE_RAILGUN:
			// Shoot laser beam
			// Emit new particles going forward in a straight line
			// Forming a type of beam
			mWeapons[mCurWeaponIndex]->FireProjectile(Weapon::TYPE_RAILGUN, mWeapons[mCurWeaponIndex]->GetNumProjectiles());
			break;
	}

	mWeapons[mCurWeaponIndex]->ResetCooldown();
	return true;
}


void Player::TakeDamage(float damage)
{
	mHealth -= damage;
}


void Player::Update(float dt, DirectInput* dInput, CollisionModel* world)
{
	// Health lower than 0, die
	if (mHealth < 0.0f)
		mIsAlive = false;

	for (UINT i = 0; i < mWeapons.size(); ++i)
		mWeapons[i]->Update(dt);

	// Move
	XMVECTOR pos = XMLoadFloat3(&mPosition);
	XMVECTOR look = XMVector3Normalize(mCamera->GetLookXM()*XMLoadFloat3(&XMFLOAT3(1,0,1)));
	if (dInput->GetKeyboardState()[DIK_W] & 0x80)
		pos += look*mSpeed*dt;
	if (dInput->GetKeyboardState()[DIK_S] & 0x80)
		pos -= look*mSpeed*dt;
	XMVECTOR right = XMVector3Normalize(mCamera->GetRightXM()*XMLoadFloat3(&XMFLOAT3(1,0,1)));
	if (dInput->GetKeyboardState()[DIK_D] & 0x80)
		pos += right*mSpeed*dt;
	if (dInput->GetKeyboardState()[DIK_A] & 0x80)
		pos -= right*mSpeed*dt;

	// Rotate camera
	if (dInput->MouseHasMoved())
	{
		float dx = XMConvertToRadians(0.25f*static_cast<float>(dInput->GetMouseState().lX));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(dInput->GetMouseState().lY));

		mCamera->Yaw(dx);
		mCamera->Pitch(dy);
	}

	if (dInput->GetMouseState().rgbButtons[0])
		Shoot();

	mCamera->UpdateViewMatrix();


	ySpeed += 300*dt;
	pos -= XMLoadFloat3(&XMFLOAT3(0,1,0))*ySpeed*dt;

	//Collision
	bool OnGround = false;
	XMVECTOR dir;
	CollisionModel::Hit hit;

	dir = XMLoadFloat3(&XMFLOAT3(0,-1,0));
	hit = world->Intersect(pos+XMLoadFloat3(&XMFLOAT3(0,20,0)), dir); 
	if(hit.hit)
	{
		//feet
		if(hit.t > 10 && hit.t < 20)
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

	dir = XMLoadFloat3(&XMFLOAT3(0,0,-1));
	hit = world->Intersect(pos+XMLoadFloat3(&XMFLOAT3(0,5,-6)), dir); 
	if(hit.hit)
	{
		//front
		if(hit.t > 6 && hit.t < 12)
			pos += dir*12-dir*hit.t;
		//back
		if(hit.t < 6)
			pos -= dir*hit.t;
	}

	dir = XMLoadFloat3(&XMFLOAT3(-1,0,0));
	hit = world->Intersect(pos+XMLoadFloat3(&XMFLOAT3(-6,5,0)), dir); 
	if(hit.hit)
	{
		//left
		if(hit.t > 6 && hit.t < 12)
			pos -= dir*12-dir*hit.t;
		//right
		if(hit.t < 6)
			pos += dir*hit.t;
	}



	//jump
	if (OnGround && dInput->GetKeyboardState()[DIK_SPACE] & 0x80)
		ySpeed = -200;


	
	XMStoreFloat3(&mPosition, pos);
	XMFLOAT3 camPos;
	XMStoreFloat3(&camPos, pos+XMLoadFloat3(&XMFLOAT3(0,10,0)));
	mCamera->SetPosition(camPos);
}



void Player::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap)
{
}
