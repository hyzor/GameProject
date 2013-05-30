#include "Game.h"

Game::Game(std::queue<PackageTo*>* send)
{
	this->send = send;
	gameActive = false;
	gameLength = 30;
	pauseLength = 10;
	
	Python->LoadModule("platform_script");
	Python->CallFunction(
		Python->GetFunction("CreatePlatforms"),
		nullptr);
	Python->Update(0.0f);
	if(Python->CheckReturns())
	{
		std::vector<int> iReturns;
		std::vector<double> dReturns;
		Python->ConvertInts(iReturns);
		Python->ConvertDoubles(dReturns);
		Python->ClearReturns();
		int iIndex = 0;
		int dIndex = 0;
		for(unsigned int i(0); i < (iReturns.size()+dReturns.size())/5; ++i)
		{
			platforms.push_back(new Platform(send, iReturns[iIndex], iReturns[iIndex+1], (float)dReturns[dIndex], (float)dReturns[dIndex+1], (float)dReturns[dIndex+2]));
			iIndex += 2;
			dIndex += 3;
		}
		iReturns.clear();
	}

	for(unsigned int i(0); i < platforms.size(); ++i)
	{
		Python->LoadModule("platform_script");
		Python->CallFunction(
			Python->GetFunction("Solution"),
			Python->CreateArg((int)i));
		Python->Update(0.0f);
		if(Python->CheckReturns())
		{
			std::vector<double> dReturns;
			Python->ConvertDoubles(dReturns);
			Python->ClearReturns();
			int index = 0;
			Python->LoadModule("player_script");
			Python->CallFunction(
				Python->GetFunction("CreateSpawnPoint"),
				Python->CreateArg(dReturns[index], dReturns[index+1], dReturns[index+2]));
			//std::cout << "( " << dReturns[index] << ", " << dReturns[index+1] << ", " << dReturns[index+2] << " )" << std::endl;
			dReturns.clear();
		}
	}

	players = new std::vector<Player*>();

	t = 0;
	this->mTimer.start();
	this->mTimer.reset();
}

Game::~Game()
{
	for(auto it(players->begin()); it != players->end(); ++it)
		if(*it) delete *it;
	delete players;

	for(auto it(platforms.begin()); it != platforms.end(); ++it)
		if(*it) delete *it;

	for(auto it(pickups.begin()); it != pickups.end(); ++it)
		if(*it) delete *it;
}

void Game::Update()
{
	mTimer.tick();
	float dt = this->mTimer.getDeltaTime();
	
	for(unsigned int i = 0; i < platforms.size(); i++)
	{
		platforms[i]->Update(dt);
		Package* p = platforms[i]->GetUpdate();
		if(p != NULL)
			send->push(new PackageTo(p, 0));
	}
	
	for(unsigned int i = 0; i < pickups.size(); i++)
	{
		pickups[i]->Update(dt);
		Package* p = pickups[i]->GetDestroy();
		if(p != NULL)
		{
			send->push(new PackageTo(p, 0));
			delete pickups[i];
			pickups.erase(pickups.begin()+i);
			break;
		}
	}

	for(unsigned int i = 0; i < players->size(); i++)
	{
		Package* p = players->at(i)->GetSelfUpdate();
		if(p != NULL)
			send->push(new PackageTo(p, (char*)players->at(i)->GetId()));
	}

	t += dt;
	if(t > 1)
	{
		t = 0;
		for(unsigned int i = 0; i < platforms.size(); i++)
		{
			Package* p = platforms[i]->GetUpdate();
			if(p != NULL)
				send->push(new PackageTo(p, 0));
		}

		if(gameActive)
		{
			if(mTimer.getTimeElapsedS() > gameLength)
			{
				gameActive = false;
				mTimer.reset();
				mTimer.start();
				Package* p = GameState();
				if(p != NULL)
					send->push(new PackageTo(p, 0));
			}
		}
		else
		{
			if( mTimer.getTimeElapsedS() > pauseLength)
			{
				gameActive = true;
				mTimer.reset();
				mTimer.start();
				Package* p = GameState();
				if(p != NULL)
					send->push(new PackageTo(p, 0));
			}
		}
		Package* p = TimeLeft();
		if(p != NULL)
			send->push(new PackageTo(p, 0));
	}
}

void Game::HandelPackage(Package* p, char* socket)
{
	if(p->GetHeader().operation == 2) // new player connected
	{
		Player* player = new Player(send,players, (int)socket, std::string(p->GetBody().Read(50)));
		players->push_back(player);
		send->push(new PackageTo(players->at(players->size()-1)->GetConnect(), 0));

		
		for(unsigned int i = 0; i < platforms.size(); i++)
			send->push(new PackageTo(platforms[i]->GetConnect(), socket));

		for(unsigned int i = 0; i < pickups.size(); i++)
			send->push(new PackageTo(pickups[i]->GetConnect(), socket));

		for(unsigned int i = 0; i < players->size() - 1; i++)
			send->push(new PackageTo(players->at(i)->GetConnect(), socket));

		std::vector<double> dReturns;
		Python->LoadModule("player_script");
		Python->CallFunction(
			Python->GetFunction("Solution"),
			nullptr);
		Python->Update(0.0f);
		if(Python->CheckReturns())
		{
			Python->ConvertDoubles(dReturns);
			Python->ClearReturns();
			player->posX = (float)dReturns[0];
			player->posY = (float)dReturns[1];
			player->posZ = (float)dReturns[2];
		}

		//player->posX = 1;
		//player->posY = 500;
		//player->posZ = 50;
		player->alive = true;
		player->health = 100;
		send->push(new PackageTo(player->GetSpawn(), 0));
		Package* spawnP = player->GetSpawn();
		spawnP->SetId(0);
		send->push(new PackageTo(spawnP, socket));
	}
	else if(p->GetHeader().operation == 1)
	{
		p->SetId((int)socket);
		Player* player = findPlayer(players, p->GetHeader().id);
		if(player != NULL)
		{
			player->HandlePackage(p);
			player->Update();
			send->push(new PackageTo(player->GetUpdate(), 0));
		}
	}
	else if(p->GetHeader().operation == 11)
	{
		p->SetId((int)socket);
		Player* player = findPlayer(players, *(int*)p->GetBody().Read(4));
		if(player != NULL)
		{
			player->HandlePackage(p);
			send->push(new PackageTo(player->GetUpdate(), 0));
		}
		send->push(new PackageTo(new Package(p), 0));
	}
	else if(p->GetHeader().operation == 9)
	{
		for(unsigned int i = 0; i < pickups.size(); i++)
		{
			if(p->GetHeader().id == pickups[i]->GetId())
			{
				pickups[i]->HandlePackage(p);

				Package::Body b = p->GetBody();
				Player* player = findPlayer(players, (int)socket);
				if(player != NULL)
					player->HandlePickup(pickups[i]);
				
				send->push(new PackageTo(pickups[i]->GetDestroy(), 0));
				delete pickups[i];
				pickups.erase(pickups.begin()+i);
				break;
			}
		}
	}
}

void Game::Disconnect(char* socket)
{
	for(unsigned int i = 0; i < players->size(); i++)
	{
		if(players->at(i)->GetId() == (int)socket)
		{
			send->push(new PackageTo(new Package(Package::Header(4, (int)socket, 0), Package::Body(new char())), 0));
			delete players->at(i);
			players->erase(players->begin() + i);
			return;
		}
	}
}



Package* Game::TimeLeft()
{
	struct timeLeft
	{
		int time;
	};
	
	timeLeft* time = new timeLeft();
	if(gameActive)
		time->time = gameLength - (int)mTimer.getTimeElapsedS();
	else
		time->time = pauseLength - (int)mTimer.getTimeElapsedS();

	return new Package(Package::Header(12, 0, sizeof(timeLeft)), Package::Body((char*)time));
}

Package* Game::GameState()
{
	struct gameState
	{
		int state;
	};
	
	gameState* state = new gameState();
	
	if(gameActive)
		state->state = 1;
	else
		state->state = 0;
		

	return new Package(Package::Header(13, 0, sizeof(gameState)), Package::Body((char*)state));
}