#include "World.h"
#include <ctime>

World::World()
	: mPlatformAmount(2), mPlatforms(std::vector<Platform*>())
{
	UINT pointLightIndex = 0;
	for (UINT i = 0; i < mPlatforms.size(); ++i)
	{
		for (UINT j = 0; j < mPlatforms[i]->mPointLights.size(); ++j)
		{
			if (pointLightIndex >= numPointLights)
				continue;

			mPointLights[pointLightIndex] = mPlatforms[i]->mPointLights[j];
			pointLightIndex++;
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

World::Hit World::Intersect(XMVECTOR origin, XMVECTOR dir, float length)
{
	Hit hit;
	hit.hit.hit = false;
	hit.hit.t = MathHelper::infinity;
	hit.platform = NULL;

	for(unsigned int i = 0; i < mPlatforms.size(); ++i)
	{
		CollisionModel::Hit hit2 = mPlatforms.at(i)->getCollision()->Intersect(origin, dir, length);
		if(hit2.hit)
		{
			if(hit2.t < hit.hit.t)
			{
				hit.platform = mPlatforms[i];
				hit.hit = hit2;
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

void World::HandlePackage(Package* p)
{
	if (p->GetHeader().operation == 5)
	{
		Package::Body b = p->GetBody();
		int type = *(int*)b.Read(4);
		XMFLOAT3 pos = *(XMFLOAT3*)b.Read(4*3);
		XMFLOAT3 mov = *(XMFLOAT3*)b.Read(4*3);

		switch(type)
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
		mPlatforms[mPlatforms.size()-1]->Initialize(p->GetHeader().id, pos);
		mPlatforms[mPlatforms.size()-1]->move = mov;
	}
	else if(p->GetHeader().operation == 6)
	{
		for(unsigned int i = 0; i < mPlatforms.size(); i++)
			if(p->GetHeader().id == mPlatforms[i]->mID)
			{
				mPlatforms[i]->HandlePackage(p);
				break;
			}
	}
}