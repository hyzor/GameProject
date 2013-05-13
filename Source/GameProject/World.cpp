#include "World.h"

World::World(int platformAmount)
	: mPlatformAmount(platformAmount), mPlatforms(std::vector<Platform*>())
{

	int platformId = 0;
	int offset = 400;
	for (unsigned int i = 0; i < mPlatformAmount; ++i)
	{
		for (unsigned int j = 0; j < mPlatformAmount; ++j)
		{
			for (unsigned int k = 0; k < mPlatformAmount; ++k)
			{
				mPlatforms.push_back(new Platform1());
				mPlatforms[platformId]->Initialize(platformId, XMFLOAT3((float)i*offset, (float)j*offset, (float)k*offset));
				platformId++;
			}
		}
	}

	//Python->LoadModule("platform_script");
	//Python->CallFunction(
	//	Python->GetFunction("CreatePlatforms"),
	//	Python->CreateArg(this->mPlatformAmount));
	//Python->Update(0.0f);
	//if(Python->CheckReturns())
	//{
	//	Python->ConvertDoubles(this->mdReturns);
	//	Python->ClearReturns();
	//	int index = 0;
	//	for(int i(0); i != mPlatformAmount; ++i)
	//	{
	//		mPlatforms.push_back(new Duck());
	//		mPlatforms.at(i)->Initialize(i, XMFLOAT3((float)mdReturns[index], (float)mdReturns[index+1], (float)mdReturns[index+2]));
	//		index += 3;
	//	}
	//	mdReturns.clear();
	//}
}

World::~World()
{
	for(unsigned int i = 0; i < mPlatforms.size(); ++i)
		SafeDelete(mPlatforms.at(i));
}

void World::Update(float dt)
{
	//Python->LoadModule("platform_script");
	//Python->Update(dt);
	//if(Python->CheckReturns())
	//{
	//	Python->ConvertDoubles(this->mdReturns);
	//	Python->ClearReturns();
	//	int index = 0;
	//	for(int i(0); i != mPlatformAmount; ++i)
	//	{
	//		mPlatforms.at(i)->Move(XMFLOAT3((float)mdReturns[index], (float)mdReturns[index+1], (float)mdReturns[index+2]));
	//		index += 3;
	//	}
	//	mdReturns.clear();
	//}
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