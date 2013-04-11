#include "Game.h"


Game::Game(ID3D11Device* device, TextureManager* mTextureMgr)
{
	mDuckModel = new GenericModel(device, mTextureMgr, "Data\\Models\\Collada\\duck.obj", L"Data\\Models\\Collada\\");
	mPlayerModel = new GenericModel(device, mTextureMgr, "Data\\Models\\OBJ\\Cop\\cop.obj", L"Data\\Models\\OBJ\\Cop\\");

	mDuck = new Entity(mDuckModel, XMFLOAT3(0, 0, 0));
	mDuckColl = new CollisionModel(mDuck->Position);
	mDuckColl->LoadObj("Data\\Models\\Collada\\duck.obj");

	mPlayer = new Player(mPlayerModel, 0, "Hyzor", XMFLOAT3(0,0,0));
}

Game::~Game()
{
	SafeDelete(mDuckModel);
	SafeDelete(mPlayerModel);
	SafeDelete(mDuck);
	SafeDelete(mPlayer);
	SafeDelete(mDuckColl);
}

void Game::Update(float deltaTime, DirectInput* di)
{
	mPlayer->Update(deltaTime, di, mDuckColl);
}

void Game::Draw(ID3D11DeviceContext* dc, ShadowMap* shadowMap)
{
	ID3DX11EffectTechnique* activeTech = Effects::BasicFX->DirLights3TexTech;
	mDuck->Draw(dc, activeTech, mPlayer->GetCamera(), shadowMap);
}

Camera* Game::GetCamera()
{
	return mPlayer->GetCamera();
}