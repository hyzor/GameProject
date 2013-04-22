#include "Game.h"

Game::Game(ID3D11Device* device, TextureManager* mTextureMgr)
{
	mDuck = new Entity(GenericHandler::GetInstance()->GetGenericModel("Duck"), XMFLOAT3(0, 0, 0)); 
	mDuckColl = new CollisionModel(mDuck->Position);
	mDuckColl->LoadObj("Data\\Models\\Collada\\duck.obj");

	mPlayer = new Player(GenericHandler::GetInstance()->GetGenericModel("Player"), 0, "Hyzor", XMFLOAT3(0,200,50));

 	mAnimatedEntity = new AnimatedEntity(GenericHandler::GetInstance()->GetGenericSkinnedModel("SkinnedModel"), XMFLOAT3(-10.0f, 60.0f, 100.0f));

	mDuck2 = new Entity(GenericHandler::GetInstance()->GetGenericModel("Duck"), XMFLOAT3(0, 0, 0)); 
	mDuckColl2 = new CollisionModel(mDuck->Position);
	mDuckColl2->LoadObj("Data\\Models\\Collada\\duck.obj");
}

Game::~Game()
{
	SafeDelete(mDuck);
	SafeDelete(mPlayer);
	SafeDelete(mDuckColl);

 	SafeDelete(mAnimatedEntity);

	SafeDelete(mDuck2);
	SafeDelete(mDuckColl2);
}

void Game::Update(float deltaTime, DirectInput* di)
{
	mPlayer->Update(deltaTime, di, mDuckColl);
	mAnimatedEntity->Update(deltaTime);
}

void Game::Draw(ID3D11DeviceContext* dc, ShadowMap* shadowMap)
{
	ID3DX11EffectTechnique* activeTech = Effects::BasicFX->DirLights3TexTech;
	mDuck->Draw(dc, activeTech, mPlayer->GetCamera(), shadowMap);

 	activeTech = Effects::NormalMapFX->DirLights3TexSkinnedTech;
 	mAnimatedEntity->Draw(dc, activeTech, mPlayer->GetCamera(), shadowMap);
}

Camera* Game::GetCamera()
{
	return mPlayer->GetCamera();
}