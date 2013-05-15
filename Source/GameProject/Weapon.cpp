#include "Weapon.h"

Weapon::Weapon()
	: mEntity(0)
{
	// Default values
	mProperties.damage = 1.0f;
	mProperties.cooldown = 5.0f;
	mProperties.currCooldown = 0.0f;
	mProperties.maxProjectiles = 1;
	mProperties.currProjectiles = 0;
}

Weapon::~Weapon()
{
	SafeDelete(mEntity);
}

void Weapon::Init(Properties properties, std::string modelName, XMFLOAT3 pos)
{
	mProperties.cooldown = properties.cooldown;
	mProperties.currCooldown = 0;
	mProperties.maxProjectiles = properties.maxProjectiles;
	mProperties.damage = properties.damage;
	mProperties.currProjectiles = 0;

	mEntity = new Entity(GenericHandler::GetInstance()->GetGenericModel(modelName), pos);
}

void Weapon::Update(float dt, float gameTime)
{
	if (mProperties.currCooldown > 0.0f)
		mProperties.currCooldown -= dt;
}

void Weapon::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, Camera* camera, ShadowMap* shadowMap)
{
	// Draw weapon model
	//mEntity->Draw(dc, tech, camera, shadowMap);
}

void Weapon::ResetCooldown()
{
	mProperties.currCooldown = mProperties.cooldown;
}

bool Weapon::FireProjectile(XMFLOAT3 pos, XMFLOAT3 dir)
{
	if (mProperties.currCooldown <= 0.0f)
	{
		// Shoot something

		// Set cooldown to initial value
		ResetCooldown();

		// Successfully fired projectile
		return true;
	}

	return false;
}

void Weapon::SetPosition(XMFLOAT3 pos)
{
	mEntity->SetPosition(pos);
}