#include "Game.h"


Game::Game(ID3D11Device* device, TextureManager* mTextureMgr)
{
	mDuckModel = new GenericModel(device, mTextureMgr, "Data\\Models\\Collada\\duck.dae", L"Data\\Models\\Collada\\");
	mPlayerModel = new GenericModel(device, mTextureMgr, "Data\\Models\\OBJ\\Cop\\cop.obj", L"Data\\Models\\OBJ\\Cop\\");
	
	mDuck = new Entity(mDuckModel, XMFLOAT3(0.0f, 0.0f, 0.0f));

	Player::InitProperties playerProp;
	playerProp.PlayerID = 0;
	playerProp.Nickname = "Hyzor";
	playerProp.Speed = 1.0f;
	playerProp.Health = 1.0f;
	playerProp.Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	playerProp.Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	playerProp.Angle = 0.0f;
	playerProp.ModelInstance.model = mPlayerModel;
	playerProp.ModelInstance.isVisible = true;

	mPlayer = new Player();
	mPlayer->Init(playerProp);
}

Game::~Game()
{
	SafeDelete(mDuckModel);
	SafeDelete(mPlayerModel);
	SafeDelete(mDuck);
	SafeDelete(mPlayer);
}

void Game::Update(float deltaTime, DirectInput* di)
{
	mPlayer->Update(deltaTime, di);
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