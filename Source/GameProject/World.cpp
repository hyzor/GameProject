#include "World.h"

World::World(int platformAmount)
	: mPlatformAmount(platformAmount), mPlatforms(std::vector<Platform*>())
{
	for(int i(0); i != mPlatformAmount; ++i)
	{
		mPlatforms.push_back(new Duck());
		mPlatforms.at(i)->Initialize(XMFLOAT3((float)i*200, (float)-i*200, (float)i*200));
	}
}

World::~World()
{
	for(int i(0); i != mPlatformAmount; ++i)
		SafeDelete(mPlatforms.at(i));
}

void World::Update(float dt)
{
	for(int i(0); i != mPlatformAmount; ++i)
		mPlatforms.at(i)->Update(dt);
}

void World::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap)
{
	for(int i(0); i != mPlatformAmount; ++i)
		mPlatforms.at(i)->Draw(dc, at, camera, shadowMap);
}

CollisionModel::Hit World::Intersect(XMVECTOR origin, XMVECTOR dir, float length)
{
	CollisionModel::Hit hit;
	hit.hit = false;
	hit.t = MathHelper::infinity;

	for(int i(0); i != mPlatformAmount; ++i)
	{
		CollisionModel::Hit hit2 = mPlatforms.at(i)->getCollision()->Intersect(origin, dir, length);
		if(hit2.hit)
		{
			if(hit2.t < hit.t)
			{
				hit = hit2;
				return hit;
			}
		}
	}
	return hit;
}