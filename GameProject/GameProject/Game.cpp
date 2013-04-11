#include "Game.h"


Game::Game(ID3D11Device* device, TextureManager* mTextureMgr)
{
	mDuckModel = new GenericModel(device, mTextureMgr, "Data\\Models\\Collada\\duck.obj", L"Data\\Models\\Collada\\");
	mPlayerModel = new GenericModel(device, mTextureMgr, "Data\\Models\\OBJ\\Cop\\cop.obj", L"Data\\Models\\OBJ\\Cop\\");

	mDuck = new Entity(mDuckModel, XMFLOAT3(0, 0, 0));
	mDuckColl = new CollisionModel(mDuck->Position);
	mDuckColl->LoadObj("Data\\Models\\Collada\\duck.obj");

	mPlayer = new Player(mPlayerModel, 0, "Hyzor", XMFLOAT3(0,200,50));

// 	mSkinnedModel = new GenericSkinnedModel(device, *mTextureMgr, "Data\\Models\\Collada\\AnimTest\\test_Collada_DAE.DAE",
// 		L"Data\\Models\\Collada\\AnimTest\\");
// 
// 	mAnimatedEntity = new AnimatedEntity(mSkinnedModel, XMFLOAT3(-50.0f, 0.0f, 0.0f));
}

Game::~Game()
{
	SafeDelete(mDuckModel);
	SafeDelete(mPlayerModel);
	SafeDelete(mDuck);
	SafeDelete(mPlayer);
	SafeDelete(mDuckColl);

// 	SafeDelete(mSkinnedModel);
// 	SafeDelete(mAnimatedEntity);
}

void Game::Update(float deltaTime, DirectInput* di)
{
	mPlayer->Update(deltaTime, di, mDuckColl);
//	mAnimatedEntity->Update(deltaTime);
}

void Game::Draw(ID3D11DeviceContext* dc, ShadowMap* shadowMap)
{
	ID3DX11EffectTechnique* activeTech = Effects::BasicFX->DirLights3TexTech;
	mDuck->Draw(dc, activeTech, mPlayer->GetCamera(), shadowMap);

// 	activeTech = Effects::NormalMapFX->DirLights3TexTech;
// 	mAnimatedEntity->Draw(dc, activeTech, mPlayer->GetCamera(), shadowMap);
}

Camera* Game::GetCamera()
{
	return mPlayer->GetCamera();
}