#include "Railgun.h"

Railgun::Railgun()
	: Weapon()
{
}

Railgun::~Railgun()
{
	delete mParticleSys;
}

void Railgun::Update(float dt)
{
	Weapon::Update(dt);
}

void Railgun::Init(Properties properties, ParticleSystem* particleSystem)
{
	Weapon::Init(properties);
	mParticleSys = particleSystem;
}

void Railgun::Draw(ID3D11DeviceContext* dc, const Camera& cam)
{
	Weapon::Draw();
	mParticleSys->draw(dc, cam);
}

bool Railgun::FireProjectile(XMFLOAT3 pos, XMFLOAT3 dir)
{
	if (Weapon::FireProjectile(pos, dir))
	{
		// Shoot ray and emit particle system

		return true;
	}

	return false;
}