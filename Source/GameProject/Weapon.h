#ifndef WEAPON_H
#define WEAPON_H
#include "d3dUtilities.h"
#include "ParticleSystem.h"

class Weapon
{
public:
	Weapon(/* Variabler för att initiera particlesystem */);
	virtual ~Weapon();
	virtual void Update(float dt);
	void Draw(/* Kod för att rita ut vapnet */);
	
	virtual void Reload() {}
	virtual void FireProjectile() = 0;
	void ResetCooldown();

	float getDamage() const { return this->mProperties.damage; }
	float getCooldown() const { return this->mProperties.currColdown; }
	int getProjectiles() const { return this->mProperties.currProjectiles; }

protected:
	struct Properties
	{
		float damage;
		float cooldown;
		float currColdown;
		int maxProjectiles;
		int currProjectiles;
	};

	Properties mProperties;

private:
	ParticleSystem mParticleSystem;
};
#endif