#include "Game.h"

Game::Game(ID3D11Device* device, TextureManager* mTextureMgr)
{
	mWorld = new World(5);
	mPlayer = new Player(GenericHandler::GetInstance()->GetGenericModel("Player"), 0, "Hyzor", XMFLOAT3(0,200,50));

 	mAnimatedEntity = new AnimatedEntity(GenericHandler::GetInstance()->GetGenericSkinnedModel("SkinnedModel"), XMFLOAT3(-10.0f, 60.0f, 100.0f));
	
	t = 0;
}

Game::~Game()
{
	SafeDelete(mPlayer);
 	SafeDelete(mAnimatedEntity);
	SafeDelete(mWorld);
}

void Game::Update(float deltaTime, DirectInput* di)
{
	mPlayer->Update(deltaTime, di, mWorld);

	mAnimatedEntity->Update(deltaTime);

	t+=deltaTime;
	if(t > 1)
	{
		t = 0;
		Network::GetInstance()->Push(new Package(Package::Header(1, 1, sizeof(XMFLOAT3)), Package::Body((char*)(new XMFLOAT3(mPlayer->GetPosition())))));
	}
}

void Game::HandlePackage(Package* p)
{
	if (p->GetHeader().operation == 1)
		mAnimatedEntity->SetPosition(*(XMFLOAT3*)p->GetBody().data);
}

void Game::Draw(ID3D11DeviceContext* dc, ShadowMap* shadowMap)
{
	ID3DX11EffectTechnique* activeTech = Effects::BasicFX->DirLights3TexTech;
	mWorld->Draw(dc, activeTech, mPlayer->GetCamera(), shadowMap);

 	activeTech = Effects::NormalMapFX->DirLights3TexSkinnedTech;
 	mAnimatedEntity->Draw(dc, activeTech, mPlayer->GetCamera(), shadowMap);
}

Camera* Game::GetCamera()
{
	return mPlayer->GetCamera();
}