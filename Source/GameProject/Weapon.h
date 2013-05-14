#ifndef WEAPON_H
#define WEAPON_H
#include "d3dUtilities.h"

class Weapon
{
public:
	struct Properties
	{
		float damage;
		float cooldown;
		float currCooldown;
		int maxProjectiles;
		int currProjectiles;
	};

protected:
	Properties mProperties;

private:
	void ResetCooldown();

public:
	Weapon();
	virtual ~Weapon();
	virtual void Update(float dt);
	virtual void Draw();

	virtual bool FireProjectile(XMFLOAT3 pos, XMFLOAT3 dir);

	virtual void Init(Properties properties);

	float getDamage() const { return this->mProperties.damage; }
	float getCooldown() const { return this->mProperties.currCooldown; }
	int getProjectiles() const { return this->mProperties.currProjectiles; }
};
#endif