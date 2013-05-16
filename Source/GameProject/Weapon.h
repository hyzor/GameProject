#ifndef WEAPON_H
#define WEAPON_H
#include "d3dUtilities.h"
#include "Camera.h"
#include "Entity.h"
#include "GenericHandler.h"

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

	Entity* mEntity;

public:
	Weapon();
	virtual ~Weapon();
	virtual void Update(float dt, float gameTime);
	virtual void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, Camera* camera, ShadowMap* shadowMap);

	void SetPosition(XMFLOAT3 pos);

	virtual bool FireProjectile(XMFLOAT3 pos, XMFLOAT3 dir);

	virtual void Init(Properties properties, std::string modelName, XMFLOAT3 pos);

	float getDamage() const { return this->mProperties.damage; }
	float getCooldown() const { return this->mProperties.currCooldown; }
	int getProjectiles() const { return this->mProperties.currProjectiles; }
};
#endif