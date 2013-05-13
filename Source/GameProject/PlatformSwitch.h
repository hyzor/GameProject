#ifndef PLATFORMSWITCH_H
#define PLATFORMSWITCH_H
#include "GenericHandler.h"
#include "CollisionModel.h"
#include "Entity.h"

class PlatformSwitch
{
public:
	PlatformSwitch();
	~PlatformSwitch();
	void Initialize(XMFLOAT3 pos);
	void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap);

private:
	CollisionModel* mCollision;
	Entity* mEntity;
};
#endif
