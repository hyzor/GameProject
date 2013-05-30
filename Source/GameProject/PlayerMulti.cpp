#include "PlayerMulti.h"

PlayerMulti::PlayerMulti(SoundModule* sm, int PlayerID, std::string Nickname, XMFLOAT3 Position, int index) : PlayerMulti::Player(sm, PlayerID, Nickname, Position, index)
{
	mModel = new AnimatedEntity(GenericHandler::GetInstance()->GetGenericSkinnedModel("SkinnedModel"), Position);
}

PlayerMulti::~PlayerMulti()
{
	delete mModel;
}

void PlayerMulti::Update(float dt, float gameTime, DirectInput* dInput, World* world, std::vector<Player*>* multiplayers)
{
	mModel->Update(dt);

	this->Player::Update(dt, gameTime, dInput, world, multiplayers);
	mModel->SetPosition(this->mPosition);
}

void PlayerMulti::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap)
{
	if(mIsAlive)
	{
		mModel->RotateXYZ(rotation, this->mCamera->Yaw-XM_PI, XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,1,0)), XMLoadFloat4x4(&Joint)));
		mModel->Draw(dc, activeTech, mCamera, shadowMap);
	}

	this->Player::Draw(dc, activeTech, mCamera, shadowMap);
}

void PlayerMulti::HandelPackage(Package *p)
{
	if (p->GetHeader().operation == 1)
	{
		Package::Body b = p->GetBody();
		this->mPosition = *(XMFLOAT3*)b.Read(4*3);
		this->move = *(XMFLOAT3*)b.Read(4*3);
		this->rotation = *(XMFLOAT3*)b.Read(4*3);
		this->mCamera->Pitch = *(float*)b.Read(4);
		this->mCamera->Roll = *(float*)b.Read(4);
		this->mCamera->Yaw = *(float*)b.Read(4);
		this->mIsAlive = (*(int*)b.Read(4))==1;
		float health = *(float*)b.Read(4);
		this->kills = *(int*)b.Read(4);
		this->deaths = *(int*)b.Read(4);

		if(mHealth > health) //hurt sound
			sm->playEnemySFX(EnemyGrunt,this->index, mPosition, false);
		mHealth = health;
	}
	else if(p->GetHeader().operation == 11)
	{
		Package::Body b = p->GetBody();
		int hitId = *(int*)b.Read(4);
		XMFLOAT3 hitPoint = *(XMFLOAT3*)b.Read(4*3);

		XMVECTOR v = XMLoadFloat3(&mPosition)+XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,15,0)), XMLoadFloat4x4(&this->Joint));
		XMFLOAT3 p;
		XMStoreFloat3(&p,v);

		XMVECTOR dir = XMVector3Normalize(v-XMLoadFloat3(&hitPoint));
		XMFLOAT3 d;
		XMStoreFloat3(&d, dir);

		if (mWeapons.at(mCurWeaponIndex)->FireProjectile(p, d))
		{
			sm->playEnemySFX(FireWeapon,this->index, p, false);
		}
	}

	Player::HandelPackage(p);
}