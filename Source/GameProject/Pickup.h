#ifndef PICKUP_H
#define PICKUP_H
#define WIN32_LEAN_AND_MEAN

#include "CollisionModel.h"
#include "Entity.h"
#include "Package.h"
#include "GenericHandler.h"
#include "Player.h"
#include <xnacollision.h>

enum TYPES
{
	EXTRA_HEALTH,
	FASTER_WEAPON
};

class Pickup
{
public:
	Pickup(void);
	~Pickup(void);
	void Initialize(int id, int type, int cpID, XMFLOAT3 pos);
	void Update(float dt, XNA::AxisAlignedBox *player, World* w);
	void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap);
	int GetId(){return mID;}
	int getCPID() const { return this->cpID;}


private:

	int mID;
	int cpID;
	int mType;
	XMFLOAT3 mPos;

	Entity* mEntity;

	std::string mModelName;
	XNA::Sphere mSphere;
};
#endif