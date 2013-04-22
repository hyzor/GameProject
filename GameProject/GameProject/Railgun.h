#ifndef RAILGUN_H
#define RAILGUN_H
#include "Weapon.h"

class Railgun : public Weapon
{
public:
	Railgun(/* Variabler för att initiera particlesystem */);
	~Railgun();
	void Update(float dt);
	void FireProjectile();
};
#endif
