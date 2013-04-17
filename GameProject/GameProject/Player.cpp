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
	rot = 0;

	this->Joint = new XMMATRIX(XMMatrixIdentity());

	mCamera = new Camera();

	mModel = model;

	SetPosition(mPosition);

	// Weapons
	Weapon* weapon = new Weapon();
	Weapon::Properties prop;
	prop.Type = Weapon::TYPE_RAILGUN;
	prop.Damage = 1.0f;                  //Objekt orienterat!!
	prop.Cooldown = 5.0f;
	prop.NumProjectiles = 1;
	weapon->Init(prop);
	mWeapons.push_back(weapon);
	mCurWeaponIndex = 0;
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
	mWeapons[mCurWeaponIndex]->FireProjectile(0,0); //Objekt orienterat!!
	/*if (mWeapons[mCurWeaponIndex]->GetCooldown() > 0.0f)
		return false;

	if(mWeapons[mCurWeaponIndex]->GetType() == Weapon::TYPE_RAILGUN)
	{
		// Shoot laser beam
		// Emit new particles going forward in a straight line
		// Forming a type of beam
		mWeapons[mCurWeaponIndex]->FireProjectile(Weapon::TYPE_RAILGUN, mWeapons[mCurWeaponIndex]->GetNumProjectiles());
	}

	mWeapons[mCurWeaponIndex]->ResetCooldown();*/
}


void Player::TakeDamage(float damage)
{
	mHealth -= damage;
}


void Player::Update(float dt, DirectInput* dInput, CollisionModel* world)
{
	XMMATRIX cJoint = *Joint;
	XMVECTOR pos = XMLoadFloat3(&mPosition);

	//switch garvity
	if(dInput->GetKeyboardState()[DIK_E] & 0x80)
	{
		if(!eDown)
		{
			rot += (float)PI/2;
			if(rot > 2*(float)PI-0.1f)
				rot = 0;
			cJoint = XMMatrixRotationX(rot);
			//mCamera->Roll(PI/2);
			ySpeed = 0;
		}
		eDown = true;
	}
	else
		eDown = false;

	// Health lower than 0, die
	if (mHealth < 0.0f)
		mIsAlive = false;

	for (UINT i = 0; i < mWeapons.size(); i++)
		mWeapons[i]->Update(dt);

	// Move
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




	//gravity
	ySpeed += 300*dt;
	pos -= XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,1,0))*ySpeed*dt, cJoint);





	//Collision
	bool OnGround = false;
	XMVECTOR dir;
	CollisionModel::Hit hit;

	/*XNA::AxisAlignedBox boundingBox;
	XMStoreFloat3(&boundingBox.Center, pos+XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,10,0)), cJoint));
	XMStoreFloat3(&boundingBox.Extents, XMVector3Transform(XMLoadFloat3(&XMFLOAT3(6,10,6)), cJoint));*/

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



	//jump
	if (OnGround && dInput->GetKeyboardState()[DIK_SPACE] & 0x80)
		ySpeed = -200;


	
	XMStoreFloat3(&mPosition, pos);
	XMFLOAT3 camPos;
	XMStoreFloat3(&camPos, pos+XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,10,0)), cJoint));
	mCamera->SetPosition(camPos);
	mCamera->UpdateViewMatrix();

	delete Joint;
	this->Joint = new XMMATRIX(cJoint);
}



void Player::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap)
{
}
