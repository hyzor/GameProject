#include "Weapon.h"


Weapon::Weapon(void)
{
	mType = -1;
	mDamage = 0.0f;
	mCooldown = 0.0f;
	mCurCooldown = 0.0f;
	mNumProjectiles = 1;
}


Weapon::~Weapon(void)
{
}

void Weapon::Init(Weapon::Properties properties)
{
	mType = properties.Type;
	mDamage = properties.Damage;
	mCooldown = properties.Cooldown;
	mNumProjectiles = properties.NumProjectiles;

	switch (mType)
	{
	case TYPE_RAILGUN:
		//mParticleSystem.init(md3d)
		break;
	}
}

int Weapon::GetType() const
{
	return mType;
}

float Weapon::GetDamage() const
{
	return mDamage;
}

float Weapon::GetCooldown() const
{
	return mCooldown;
}

void Weapon::Update(float dt)
{
	// Decrease weapons current cooldown
	if (mCurCooldown > 0.0f)
		mCurCooldown -= dt*0.01f;
}

void Weapon::ResetCooldown()
{
	mCurCooldown = mCooldown;
}

void Weapon::FireProjectile( int type, UINT numProjectiles )
{
	switch (type)
	{
	case TYPE_RAILGUN:

		for (UINT i = 0; i < numProjectiles; ++i)
		{

		}

		break;
	}
}

UINT Weapon::GetNumProjectiles() const
{
	return mNumProjectiles;
}
