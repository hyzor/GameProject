#ifndef WORLD_H
#define WORLD_H
#include <vector>
#include "Duck.h"

class World
{
public:
	World(int platformAmount);
	~World(void);
	void Update(float dt);
	void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap);
	CollisionModel::Hit Intersect(XMVECTOR origin, XMVECTOR dir, float length);

private:
	int mPlatformAmount;
	std::vector<Platform*> mPlatforms;
};
#endif