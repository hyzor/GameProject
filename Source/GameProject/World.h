#ifndef WORLD_H
#define WORLD_H
#include <vector>
#include "Duck.h"
#include "PyEngine.h"
#include "Platform1.h"
#include "Platform2.h"
#include "Platform3.h"
#include "Platform4.h"

class World
{
public:
	World();
	~World(void);
	void Update(float dt);
	void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap);
	CollisionModel::Hit Intersect(XMVECTOR origin, XMVECTOR dir, float length);
	PlatformSwitch* IntersectSwitch(XMVECTOR origin, XMVECTOR dir, float length);

private:
	int mPlatformAmount;
	std::vector<Platform*> mPlatforms;
	std::vector<double> mdReturns;
};
#endif