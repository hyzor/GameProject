#include "Player.h"


Player::Player(GenericModel* model, int PlayerID, std::string Nickname, XMFLOAT3 Position)
{

	mInCameraFrustum = true;

	mHealth = 1;
	mNickname = Nickname;
	mSpeed = 100;
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
	if (dInput->GetKeyboardState()[DIK_W] && 0x80)
		pos += mCamera->GetLookXM()*mSpeed*dt;
	if (dInput->GetKeyboardState()[DIK_S] & 0x80)
		pos -= mCamera->GetLookXM()*mSpeed*dt;
	if (dInput->GetKeyboardState()[DIK_D] & 0x80)
		pos += mCamera->GetRightXM()*mSpeed*dt;
	if (dInput->GetKeyboardState()[DIK_A] & 0x80)
		pos -= mCamera->GetRightXM()*mSpeed*dt;

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


	//Collision
	XMVECTOR dir = XMLoadFloat3(&XMFLOAT3(0,-10,0));
	CollisionModel::Hit hit = world->Intersect(pos+XMLoadFloat3(&XMFLOAT3(0,-10,0)), dir); 
	if(hit.hit)
	{
		//pos -= dir;
		//feet
		if(hit.dot > 0 && hit.t > 0.5f)
			pos -= dir-dir*hit.t;
		//head
		else if(hit.dot < 0 && hit.t < 0.5f)
			pos += dir*hit.t;
	}

	
	XMStoreFloat3(&mPosition, pos);
	mCamera->SetPosition(mPosition);
}



void Player::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap)
{
}
