#include "World.h"
#include <ctime>

World::World()
	: mPlatformAmount(2), mPlatforms(std::vector<Platform*>())
{
	srand(unsigned int(time(nullptr)));
	int platformId = 0;
	int offset = 400;
	int random = 0;
	for (int i = 0; i < mPlatformAmount; ++i)
	{
		for (int j = 0; j < mPlatformAmount; ++j)
		{
			for (int k = 0; k < mPlatformAmount; ++k)
			{
				random = 1 ; //rand()%4 + 1;
				switch(random)
				{
				case 1:
					mPlatforms.push_back(new Platform1());
					break;
				case 2:
					mPlatforms.push_back(new Platform2());
					break;
				case 3:
					mPlatforms.push_back(new Platform3());
					break;
				case 4:
					mPlatforms.push_back(new Platform4());
					break;
				}
				mPlatforms[platformId]->Initialize(platformId, XMFLOAT3((float)i*offset, (float)j*offset, (float)k*offset));
				platformId++;
			}
		}
	}
}

World::~World()
{
	for(unsigned int i = 0; i < mPlatforms.size(); ++i)
		SafeDelete(mPlatforms.at(i));
}

void World::Update(float dt)
{
	for(unsigned int i = 0; i < mPlatforms.size(); ++i)
		mPlatforms.at(i)->Update(dt);
}

void World::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap)
{
	for(unsigned int i = 0; i < mPlatforms.size(); ++i)
		mPlatforms.at(i)->Draw(dc, at, camera, shadowMap);
}

CollisionModel::Hit World::Intersect(XMVECTOR origin, XMVECTOR dir, float length)
{
	CollisionModel::Hit hit;
	hit.hit = false;
	hit.t = MathHelper::infinity;

	for(unsigned int i = 0; i < mPlatforms.size(); ++i)
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

PlatformSwitch* World::IntersectSwitch(XMVECTOR origin, XMVECTOR dir, float length)
{
	for(UINT i = 0; i < mPlatforms.size(); i++)
	{
		PlatformSwitch* s = mPlatforms[i]->IntersectSwitch(origin, dir, length);
		if(s != NULL)
			return s;
	}
	return NULL;
}