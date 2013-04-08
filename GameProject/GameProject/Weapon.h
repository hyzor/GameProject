#ifndef WEAPON_H_
#define WEAPON_H_

#include "d3dUtilities.h"
#include "ParticleSystem.h"

class Weapon
{
public:
	Weapon(void);
	~Weapon(void);

	enum TypeList
	{
		TYPE_RAILGUN = 0
	};

	struct Properties
	{
		int Type;
		float Damage;
		float Cooldown;
		int NumProjectiles;
	};

	void Update(float dt);

	void FireProjectile(int type, int numProjectiles);

	void Init(Weapon::Properties properties);

	void ResetCooldown();

	int GetType() const;
	float GetDamage() const;
	float GetCooldown() const;
	UINT GetNumProjectiles() const;

private:
	int mType;
	float mDamage;
	float mCooldown;
	float mCurCooldown;
	UINT mNumProjectiles;

	ParticleSystem mParticleSystem;
};

#endif