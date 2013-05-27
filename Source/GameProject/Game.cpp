#include "Game.h"

Game::Game(ID3D11Device* device, ID3D11DeviceContext* dc, TextureManager* mTextureMgr)
{
	world = new World();

	player = new PlayerLocal("Hyzor", XMFLOAT3(1,300,50));
	player->InitWeapons(device, dc);
	this->device = device;
	this->dc = dc;

	multiplayers = new std::vector<Player*>();

	this->mPickup = new Pickup();
	XMFLOAT3 pickupPos(player->GetPosition().x, player->GetPosition().y+150, player->GetPosition().z);
	this->mPickup->Initialize(0, EXTRA_HEALTH, pickupPos);

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

	SafeDelete(this->mPickup);
}

void Game::Update(float deltaTime, float gameTime, DirectInput* di, SoundModule* sm)
{
	player->Update(deltaTime, gameTime, di, sm, world);
	for(UINT i = 0; i < multiplayers->size(); i++)
		multiplayers->at(i)->Update(deltaTime, gameTime, di, sm, world);

	animatedEntity->Update(deltaTime);
	world->Update(deltaTime);

	mPickup->Update(deltaTime);
	if(mPickup->Intersect(player->GetPosition()))
	{
		// Saker som ska hända när pickupen kolliderar med spelaren
	}
}

void Game::HandlePackage(Package* p)
{
	int o = p->GetHeader().operation;
	if(o == 2)
	{
		Package::Body b = p->GetBody();
		XMFLOAT3 pos = *(XMFLOAT3*)b.Read(4*3);
		float h = *(float*)b.Read(4);
		int a = *(int*)b.Read(4);
		PlayerMulti* pm = new PlayerMulti(p->GetHeader().id, std::string(b.Read(50)), pos);
		pm->mHealth = h;
		pm->mIsAlive = a==1;
		pm->InitWeapons(this->device, this->dc);
		multiplayers->push_back(pm);
	}
	else if(o == 1 || o == 3)
	{
		if(p->GetHeader().id == 0)
			player->HandelPackage(p);
		else
			for(UINT i = 0; i < multiplayers->size(); i++)
				if(multiplayers->at(i)->GetID() == p->GetHeader().id)
				{
					multiplayers->at(i)->HandelPackage(p);
					break;
				}
	}
	else if(o == 4)
	{
		for(UINT i = 0; i< multiplayers->size(); i++)
			if(multiplayers->at(i)->GetID() == p->GetHeader().id)
				multiplayers->erase(multiplayers->begin() + i);
	}
	else if(o == 5 || o == 6)
		world->HandlePackage(p);

}

void Game::Draw(ID3D11DeviceContext* dc, ShadowMap* shadowMap)
{
	// Set point lights created in world
	Effects::NormalMapFX->SetPointLights(world->mPointLights);

	// Draw world and player with dir lights and point lights
	ID3DX11EffectTechnique* activeTech = Effects::NormalMapFX->DirLights3PointLights12TexAlphaClipTech;
	world->Draw(dc, activeTech, player->GetCamera(), shadowMap);
	mPickup->Draw(dc, activeTech, player->GetCamera(), shadowMap);
	player->Draw(dc, activeTech, player->GetCamera(), shadowMap);

 	activeTech = Effects::NormalMapFX->DirLights3TexSkinnedTech;
 	animatedEntity->Draw(dc, activeTech, player->GetCamera(), shadowMap);
	
	for(UINT i = 0; i < multiplayers->size(); i++)
		multiplayers->at(i)->Draw(dc, activeTech, player->GetCamera(), shadowMap);
}

Camera* Game::GetCamera()
{
	return player->GetCamera();
}

Player* Game::GetPlayer() const
{
	return player;
}