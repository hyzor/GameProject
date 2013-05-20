#include "Game.h"

Game::Game(std::queue<PackageTo*>* send)
{
	this->send = send;

	//load world
	platforms.push_back(new Platform(send, 0, 0, 0, 0, 0));
}

Game::~Game()
{
	for(int i = 0; i < players.size(); i++)
		delete players[i];
	for(int i = 0; i < platforms.size(); i++)
		delete platforms[i];
}

void Game::Update()
{
	for(int i = 0; i < platforms.size(); i++)
	{
		platforms[i]->Update();
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

		for(int i = 0; i < players.size() - 1; i++)
			send->push(new PackageTo(players[i]->GetConnect(), socket));

		for(int i = 0; i < platforms.size(); i++)
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
	for(int i = 0; i < players.size(); i++)
		if(players[i]->GetId() == (int)socket)
		{
			send->push(new PackageTo(new Package(Package::Header(4, (int)socket, 0), Package::Body(new char())), 0));
			players.erase(players.begin() + i);
			return;
		}
}

Player* Game::findPlayer(int id)
{
	for(int i = 0; i < players.size(); i++)
		if(players[i]->GetId() == id)
			return players[i];
	return NULL;
}