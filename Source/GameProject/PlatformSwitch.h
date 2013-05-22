#ifndef PLATFORMSWITCH_H
#define PLATFORMSWITCH_H
#include "GenericHandler.h"
#include "CollisionModel.h"
#include "Entity.h"

struct SwitchRotations
{
	XMFLOAT3 start;
	XMFLOAT3 rot;
};

class PlatformSwitch
{
public:
	PlatformSwitch();
	~PlatformSwitch();
	void Initialize(XMFLOAT3 pos, int type, int rotationType, XMFLOAT4X4 rot, XMFLOAT3 collisionOffset);
	void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap);
	Entity* getEntity() const;
	CollisionModel* getCollision() { return this->mCollision; }
	XMVECTOR GetMoveTo(XMVECTOR up);
	float GetRotationX(XMVECTOR up);
	float GetRotationZ(XMVECTOR up);
	float GetRotationY(XMVECTOR up);
	SwitchRotations GetRotations(XMVECTOR up);

private:
	CollisionModel* mCollision;
	Entity* mEntity;
	int rotationType;
};
#endif
