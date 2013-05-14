#include "Weapon.h"

Weapon::Weapon()
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
}

void Weapon::Init(Properties properties)
{
	mProperties.cooldown = properties.cooldown;
	mProperties.currCooldown = 0;
	mProperties.maxProjectiles = properties.maxProjectiles;
	mProperties.damage = properties.damage;
	mProperties.currProjectiles = 0;
}

void Weapon::Update(float dt)
{
	if (mProperties.currCooldown > 0.0f)
		mProperties.currCooldown -= dt;
}

void Weapon::Draw()
{
	// Draw weapon model
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