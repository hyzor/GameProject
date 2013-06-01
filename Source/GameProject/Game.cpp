#include "Game.h"

Game::Game(ID3D11Device* device, ID3D11DeviceContext* dc, TextureManager* mTextureMgr, SoundModule* sm)
{
	world = new World();
	this->sm = sm;
	multiplayers = new std::vector<Player*>();

	player = new PlayerLocal(sm, Settings::GetInstance()->GetData().PlayerName, XMFLOAT3(1,300,50), multiplayers);
	player->InitWeapons(device, dc);
	this->device = device;
	this->dc = dc;
	gameActive = true;
	timeLeft = 0;
	pauseTimeLeft = 0;

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
	for(UINT i = 0; i < pickups.size(); i++)
		SafeDelete(this->pickups[i]);
}

void Game::Update(float deltaTime, float gameTime, DirectInput* di)
{
	player->Update(deltaTime, gameTime, di, world, multiplayers);
	for(UINT i = 0; i < multiplayers->size(); i++)
		multiplayers->at(i)->Update(deltaTime, gameTime, di, world, multiplayers);

	animatedEntity->Update(deltaTime);
	world->Update(deltaTime);

	for(UINT i = 0; i < pickups.size(); i++)
		pickups[i]->Update(deltaTime, &player->GetBounding(), this->getWorld());
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
		PlayerMulti* pm = new PlayerMulti(sm, p->GetHeader().id, std::string(b.Read(50)), pos, multiplayers, multiplayers->size());
		sm->addEnemy(pm->index);
		pm->mHealth = h;
		pm->mIsAlive = a==1;
		pm->InitWeapons(this->device, this->dc);
		multiplayers->push_back(pm);
	}
	else if(o == 1 || o == 3 || o == 10 || o == 11)
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
			{
				SafeDelete(multiplayers->at(i));
				multiplayers->erase(multiplayers->begin() + i);
				break;
			}
	}
	else if(o == 5 || o == 6)
		world->HandlePackage(p);
	else if(o == 7)
	{
		Package::Body b = p->GetBody();
		int type = *(int*)b.Read(4);
		int cpID = *(int*)b.Read(4);
		XMFLOAT3 p = *(XMFLOAT3*)b.Read(3*4);
		Pickup* pickup = new Pickup();
		pickup->Initialize(0, type, cpID, p);
		pickups.push_back(pickup);
	}
	else if(o == 8)
	{
		for(unsigned int i = 0; i < pickups.size(); i++)
			if(p->GetHeader().id == pickups[i]->GetId())
			{
				delete pickups[i];
				pickups.erase(pickups.begin()+i);
				break;
			};
	}
	else if(o == 12)
	{
		Package::Body b = p->GetBody();
		if(gameActive)
			timeLeft = *(int*)b.Read(4);
		else
			pauseTimeLeft = *(int*)b.Read(4);
	}
	
	else if(o == 13)
	{
		Package::Body b = p->GetBody();
		int result = *(int*)b.Read(4);
		
		if(result == 1)
		{
			this->ResetGame();
			gameActive = true;
			Gui->setState(GUI::Game);
		}
		else 
		{
			gameActive = false;
			Gui->setState(GUI::Pause);
		}
	}
}

void Game::Draw(ID3D11DeviceContext* dc, ShadowMap* shadowMap)
{
	// Set point lights created in world
	Effects::NormalMapFX->SetPointLights(world->mPointLights);

	// Draw world and player with dir lights and point lights
	ID3DX11EffectTechnique* activeTech = Effects::NormalMapFX->DirLights3PointLights12TexAlphaClipTech;
	world->Draw(dc, activeTech, player->GetCamera(), shadowMap);

	// --- Remove ---
	activeTech = Effects::NormalMapFX->DirLights3TexSkinnedTech;
	animatedEntity->Draw(dc, activeTech, player->GetCamera(), shadowMap);

	// Draw pickups
	activeTech = Effects::NormalMapFX->DirLights3PointLights12TexAlphaClipTech;
	for(UINT i = 0; i < pickups.size(); i++)
		pickups[i]->Draw(dc, activeTech, player->GetCamera(), shadowMap);
	
	// Draw local player
	player->Draw(dc, activeTech, player->GetCamera(), shadowMap);

	// Draw multiplayers
	activeTech = Effects::NormalMapFX->DirLights3TexSkinnedTech;
	for(UINT i = 0; i < multiplayers->size(); i++)
		multiplayers->at(i)->Draw(dc, activeTech, player->GetCamera(), shadowMap);

	wstringstream wss;
	if(gameActive)
	{
		wss<< "Time Left: " << timeLeft;
		Gui->drawText(dc, (wchar_t*)wss.str().c_str(), XMFLOAT2(20,20), 25, 0xff0000ff);
	}
	else
	{
		wss<<"Time to next game: " << pauseTimeLeft;
		Gui->drawText(dc, (wchar_t*)wss.str().c_str(), XMFLOAT2(20,20), 25, 0xff0000ff);
	}
}

Camera* Game::GetCamera()
{
	return player->GetCamera();
}

Player* Game::GetPlayer() const
{
	return player;
}

void Game::ResetGame()
{
	Python->LoadModule("scoreboard_script");
	Python->CallFunction(
		Python->GetFunction("ResetStats"),
		nullptr);
}