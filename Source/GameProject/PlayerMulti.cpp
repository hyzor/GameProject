#include "PlayerMulti.h"
#include "GUI.h"

PlayerMulti::PlayerMulti(SoundModule* sm, int PlayerID, std::string Nickname, XMFLOAT3 Position, std::vector<Player*>* multiplayers, int index) : PlayerMulti::Player(sm, PlayerID, Nickname, Position, multiplayers, index)
{
	mEntity = new AnimatedEntity(GenericHandler::GetInstance()->GetGenericSkinnedModel("SkinnedModel"), Position);
}

PlayerMulti::~PlayerMulti()
{
	delete mEntity;
}

void PlayerMulti::Update(float dt, float gameTime, DirectInput* dInput, World* world, std::vector<Player*>* multiplayers)
{
	mEntity->Update(dt);

	this->Player::Update(dt, gameTime, dInput, world, multiplayers);
	mEntity->SetPosition(this->mPosition);
}

void PlayerMulti::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap)
{
	if(mIsAlive)
	{
		mEntity->RotateXYZ(rotation, this->mCamera->Yaw-XM_PI, XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,1,0)), XMLoadFloat4x4(&Joint)));
		mEntity->Draw(dc, activeTech, mCamera, shadowMap);
	}

	this->Player::Draw(dc, activeTech, mCamera, shadowMap);

	activeTech = Effects::NormalMapFX->DirLights3TexAlphaClipTech;
	// Draw weapon (weapon model is excluded)
	mWeapons[mCurWeaponIndex]->Draw(dc, activeTech, mCamera, shadowMap, false);
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
		bool alive = (*(int*)b.Read(4))==1;
		float health = *(float*)b.Read(4);
		int kills = *(int*)b.Read(4);
		int deaths = *(int*)b.Read(4);
		this->deathBy = *(int*)b.Read(4);

		this->setKillsDeaths(kills, deaths);

		if(mHealth > health) //hurt sound
			sm->playEnemySFX(EnemyGrunt,this->index, mPosition, false);
		mHealth = health;

		if(this->mIsAlive && !alive) //killed gui text
		{
			wstringstream wss;
			if(this->deathBy == 0)
				wss << this->mNickname.c_str() << " died from falling!";
			else
			{
				bool found = false;
				for(unsigned int i = 0; i < this->multiplayers->size(); i++)
					if(multiplayers->at(i)->GetID() == deathBy)
					{
						wss << multiplayers->at(i)->mNickname.c_str();
						found = true;
						break;
					};
				if(!found)
					wss << "You killed " << this->mNickname.c_str() << "!";
				else
					wss << " killed " << this->mNickname.c_str();
			}
			GUI::GetInstance()->AddEventText(wss.str(), 4);
		}
		this->mIsAlive = alive;
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

		if (mWeapons.at(mCurWeaponIndex)->FireProjectile(p, XMFLOAT3(-d.x, -d.y, -d.z)))
		{
			sm->playEnemySFX(FireWeapon,this->index, p, false);
		}
	}

	Player::HandelPackage(p);
}