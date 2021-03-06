#ifndef RAILGUN_H
#define RAILGUN_H
#include "Weapon.h"
#include "ParticleSystem.h"
#include "Common\Camera.h"
#include "Effects.h"
#include "d3dUtilities.h"

class Railgun : public Weapon
{
public:
	Railgun();
	~Railgun();
	void Init(Properties properties, ID3D11Device* device, ID3D11DeviceContext* dc, std::string modelName, XMFLOAT3 pos);
	void Update(float dt, float gameTime);
	void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, Camera* camera, ShadowMap* shadowMap, bool isVisible);
	bool FireProjectile(XMFLOAT3 pos, XMFLOAT3 dir, XMFLOAT3 hitPos);

private:

	//------------------------------------
	// Particle data
	//------------------------------------
	ParticleSystem* mParticleSys;

	ID3D11ShaderResourceView* mLaserTexSRV;
	ID3D11ShaderResourceView* mRandomTexSRV;
};
#endif
