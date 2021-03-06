#ifndef WORLD_H
#define WORLD_H
#include <vector>
#include "Duck.h"
#include "PyEngine.h"
#include "Platform1.h"
#include "Platform2.h"
#include "Platform3.h"
#include "Platform4.h"
#include "Package.h"

class World
{
public:
	struct Hit
	{
		CollisionModel::Hit hit;
		Platform* platform;
	};

	World();
	~World(void);
	void Update(float dt);
	void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap);
	Hit Intersect(XMVECTOR origin, XMVECTOR dir, float length);
	PlatformSwitch* IntersectSwitch(XMVECTOR origin, XMVECTOR dir, float length);
	Platform* getPlatform(int id);

	void HandlePackage(Package *p);

	static const UINT numPointLights = 12;
	PointLight mPointLights[numPointLights];
	UINT mPointLightIndex;
	//std::vector<PointLight*> mPointLights;

private:
	int mPlatformAmount;
	std::vector<Platform*> mPlatforms;
	std::vector<double> mdReturns;
};
#endif