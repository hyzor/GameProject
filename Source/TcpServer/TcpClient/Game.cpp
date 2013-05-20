#include "Game.h"

Game::Game(std::queue<PackageTo*>* send)
{
	this->send = send;
	this->mTimer.start();
	this->mTimer.reset();

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
}

Game::~Game()
{
	for(unsigned int i = 0; i < players.size(); i++)
		delete players[i];
	for(unsigned int i = 0; i < platforms.size(); i++)
		delete platforms[i];
}

void Game::Update()
{
	for(unsigned int i = 0; i < platforms.size(); i++)
	{
		platforms[i]->Update(this->mTimer.getDeltaTime());
		Package* p = platforms[i]->GetUpdate();
		if(!p == NULL)
			send->push(new PackageTo(p, 0));
	}
}

void Game::HandelPackage(Package* p, char* socket)
{
	if(p->GetHeader().operation == 2) // new player connected
	{
		players.push_back(new Player(send, (int)socket, std::string(p->GetBody().Read(50))));
		send->push(new PackageTo(players[players.size()-1]->GetConnect(), 0));

		for(unsigned int i = 0; i < players.size() - 1; i++)
			send->push(new PackageTo(players[i]->GetConnect(), socket));

		for(unsigned int i = 0; i < platforms.size(); i++)
			send->push(new PackageTo(platforms[i]->GetConnect(), socket));
	}
	else if(p->GetHeader().operation == 1 || p->GetHeader().operation == 3)
	{
		p->SetId((int)socket);
		Player* player = findPlayer(p->GetHeader().id);
		if(player != NULL)
		{
			player->HandelPackage(p);
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