#include "Railgun.h"

Railgun::Railgun(/* Variabler för att initiera particlesystem */)
	: Weapon()
{
	mProperties.damage = MathHelper::infinity; // Huehue
	mProperties.cooldown = 5.0f;
	mProperties.currColdown = 0.0f;
	mProperties.maxProjectiles = 1;
	mProperties.currProjectiles = mProperties.maxProjectiles;
}

Railgun::~Railgun(void)
{
}

void Railgun::Update(float dt)
{
	Weapon::Update(dt);
}

void Railgun::FireProjectile()
{
	// Pew pew
	Weapon::ResetCooldown();
}