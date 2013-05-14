#ifndef RAILGUN_H
#define RAILGUN_H
#include "Weapon.h"
#include "ParticleSystem.h"
#include "Common\Camera.h"

class Railgun : public Weapon
{
public:
	Railgun();
	~Railgun();
	void Init(Properties properties, ParticleSystem* particleSystem);
	void Update(float dt);
	void Draw(ID3D11DeviceContext* dc, const Camera& cam);
	bool FireProjectile(XMFLOAT3 pos, XMFLOAT3 dir);

private:
	ParticleSystem* mParticleSys;
};
#endif
