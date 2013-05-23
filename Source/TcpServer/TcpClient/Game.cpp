#include "Game.h"

Game::Game(std::queue<PackageTo*>* send)
{
	this->send = send;

	Python->LoadModule("platform_script");
	Python->CallFunction(
		Python->GetFunction("CreatePlatforms"),
		nullptr);
	Python->Update(0.0f);
	if(Python->CheckReturns())
	{
		std::vector<int> iReturns;
		Python->ConvertInts(iReturns);
		Python->ClearReturns();
		int index = 0;
		for(unsigned int i(0); i < iReturns.size()/5; ++i)
		{
			platforms.push_back(new Platform(send, iReturns[index], iReturns[index+1], (float)iReturns[index+2], (float)iReturns[index+3], (float)iReturns[index+4]));
			index += 5;
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

	t = 0;
	this->mTimer.start();
	this->mTimer.reset();
}

Game::~Game()
{
	for(auto it(players.begin()); it != players.end(); ++it)
		if(*it) delete *it;

	for(auto it(platforms.begin()); it != platforms.end(); ++it)
		if(*it) delete *it;

	for(auto it(pickups.begin()); it != pickups.end(); ++it)
		if(*it) delete *it;
}

void Game::Update()
{
	float dt = this->mTimer.getDeltaTime();

	t += dt;
	if(t > 1)
	{
		t = 0;
		for(unsigned int i = 0; i < platforms.size(); i++)
		{
			platforms[i]->Update(dt);
			Package* p = platforms[i]->GetUpdate();
			if(!p == NULL)
				send->push(new PackageTo(p, 0));
		}
	}

}

void Game::HandelPackage(Package* p, char* socket)
{
	if(p->GetHeader().operation == 2) // new player connected
	{
		Player* player = new Player(send, (int)socket, std::string(p->GetBody().Read(50)));
		players.push_back(player);
		send->push(new PackageTo(players[players.size()-1]->GetConnect(), 0));

		for(unsigned int i = 0; i < players.size() - 1; i++)
			send->push(new PackageTo(players[i]->GetConnect(), socket));

		for(unsigned int i = 0; i < platforms.size(); i++)
			send->push(new PackageTo(platforms[i]->GetConnect(), socket));

		//first player spawn
		player->posX = 1;
		player->posY = 400;
		player->posZ = 50;
		player->alive = true;
		player->health = 100;
		send->push(new PackageTo(player->GetSpawn(), 0));
		Package* spawnP = player->GetSpawn();
		spawnP->SetId(0);
		send->push(new PackageTo(spawnP, socket));
	}
	else if(p->GetHeader().operation == 1 || p->GetHeader().operation == 3)
	{
		p->SetId((int)socket);
		Player* player = findPlayer(p->GetHeader().id);
		if(player != NULL)
		{
			player->HandelPackage(p);
			player->Update();
			send->push(new PackageTo(player->GetUpdate(), 0));
		}
	}
}

void Game::Disconnect(char* socket)
{
	for(unsigned int i = 0; i < players.size(); i++)
	{
		if(players[i]->GetId() == (int)socket)
		{
			send->push(new PackageTo(new Package(Package::Header(4, (int)socket, 0), Package::Body(new char())), 0));
			players.erase(players.begin() + i);
			return;
		}
	}
}

Player* Game::findPlayer(int id)
{
	for(unsigned int i = 0; i < players.size(); i++)
		if(players[i]->GetId() == id)
			return players[i];
	return nullptr;
}

void Game::CheckPickups()
{
	/*
	Funktionen här bör kallas varje update för att kolla scriptet och skapa pickups
	*/
}