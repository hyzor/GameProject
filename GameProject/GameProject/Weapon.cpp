#include "Weapon.h"

Weapon::Weapon(/* Variabler för att initiera particlesystem */)
{
	// Initiering av mParticleSystem
}

Weapon::~Weapon()
{
}

void Weapon::Update(float dt)
{
	if (mProperties.currColdown > 0.0f)
		mProperties.currColdown -= dt*0.01f;
}

void Weapon::Draw()
{
}

void Weapon::ResetCooldown()
{
	mProperties.currColdown = mProperties.cooldown;
}