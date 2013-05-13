#include "Game.h"

Game::Game(std::queue<PackageTo*>* send)
{
	this->send = send;
}

Game::~Game()
{
	for(int i = 0; i < players.size(); i++)
		delete players[i];
}

void Game::Update()
{
}

void Game::HandelPackage(Package* p, char* socket)
{
	if(p->GetHeader().operation == 2)
	{
		players.push_back(new Player((int)socket, std::string(p->GetBody().Read(50))));
		send->push(new PackageTo(players[players.size()-1]->GetConnect(), 0));

		for(int i = 0; i < players.size() - 1; i++)
			send->push(new PackageTo(players[i]->GetConnect(), socket));
	}
	else if(p->GetHeader().operation == 1 || p->GetHeader().operation == 3)
	{
		p->SetId((int)socket);
		Player* player = findPlayer(p->GetHeader().id);
		player->HandelPackage(p);
		send->push(new PackageTo(player->GetUpdate(), 0));
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
}