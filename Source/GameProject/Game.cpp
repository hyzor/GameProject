#include "Game.h"

Game::Game(ID3D11Device* device, TextureManager* mTextureMgr)
{
	world = new World();

	player = new PlayerLocal("Hyzor", XMFLOAT3(1,300,50));
	multiplayers = new std::vector<Player*>();

 	animatedEntity = new AnimatedEntity(GenericHandler::GetInstance()->GetGenericSkinnedModel("SkinnedModel"), XMFLOAT3(-10.0f, 60.0f, 100.0f));
}

Game::~Game()
{
	SafeDelete(player);
 	SafeDelete(animatedEntity);
	SafeDelete(world);
	for(UINT i = 0; i < multiplayers->size(); i++)
		SafeDelete(multiplayers->at(i));
	SafeDelete(multiplayers);
}

void Game::Update(float deltaTime, DirectInput* di, SoundModule* sm)
{
	player->Update(deltaTime, di, sm, world);
	for(UINT i = 0; i < multiplayers->size(); i++)
		multiplayers->at(i)->Update(deltaTime, di, sm, world);

	animatedEntity->Update(deltaTime);

	world->Update(deltaTime);
}

void Game::HandlePackage(Package* p)
{
	if(p->GetHeader().operation == 2)
		multiplayers->push_back(new PlayerMulti(p->GetHeader().id, std::string(p->GetBody().Read(50)), XMFLOAT3(0,0,0)));
	else
	{
		if(p->GetHeader().id == 0)
			player->HandelPackage(p);
		else
			for(UINT i = 0; i < multiplayers->size(); i++)
				if(multiplayers->at(i)->GetID() == p->GetHeader().id)
					multiplayers->at(i)->HandelPackage(p);
	}
}

void Game::Draw(ID3D11DeviceContext* dc, ShadowMap* shadowMap)
{
	ID3DX11EffectTechnique* activeTech = Effects::NormalMapFX->DirLights3TexAlphaClipTech;
	world->Draw(dc, activeTech, player->GetCamera(), shadowMap);

 	activeTech = Effects::NormalMapFX->DirLights3TexSkinnedTech;
 	animatedEntity->Draw(dc, activeTech, player->GetCamera(), shadowMap);
	
	for(UINT i = 0; i < multiplayers->size(); i++)
		multiplayers->at(i)->Draw(dc, activeTech, player->GetCamera(), shadowMap);
}

Camera* Game::GetCamera()
{
	return player->GetCamera();
}
