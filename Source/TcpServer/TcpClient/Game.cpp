#include "Game.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

Game::Game(std::queue<PackageTo*>* send)
{
	this->nrOfPickups = 0;
	this->send = send;
	gameActive = false;
	gameLength = 360;
	pauseLength = 3;
	
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


	players = new std::vector<Player*>();

	std::vector<float> xPos;
	std::vector<float> yPos;
	std::vector<float> zPos;
	std::vector<int> pID;

	//extract positions from platforms to create suitable spawnpositions for the pickups

	for(unsigned int i = 0; i < this->platforms.size(); i++)
	{
		int pType = this->platforms.at(i)->getType();
		switch(pType)
		{
		case 1:
			xPos.push_back(50.0f);
			yPos.push_back(30.0f);
			zPos.push_back(-75.f);
			break;
		case 2:
			xPos.push_back(-40.0f);
			yPos.push_back(48.0f);
			zPos.push_back(-5.0f);
			break;
		case 3:
			xPos.push_back(58.0f);
			yPos.push_back(46.0f);
			zPos.push_back(0.0f);
			break;
		case 4:
			xPos.push_back(0.0f);
			yPos.push_back(8.0f);
			zPos.push_back(37.0f);
			break;
			
		}
		pID.push_back(this->platforms.at(i)->getID());
	}

	srand((unsigned int)time(NULL));

	Python->LoadModule("pickup_script");
	for(unsigned int i = 0; i < xPos.size(); i++) // 1 spawnpoint per platform, for now
	{
		Python->CallFunction( Python->GetFunction("pCreateSpawnPoint"), Python->CreateArg((double)xPos.at(i), (double)yPos.at(i), (double)zPos.at(i)) );
	}

	int nrOfSPs = xPos.size();
	int randomizedSpawnPos = rand() % nrOfSPs;
	Python->CallFunction( Python->GetFunction("createPickup"), Python->CreateArg(this->platforms.at(randomizedSpawnPos)->getID(), randomizedSpawnPos, nrOfSPs) ); 

	std::vector<int> iReturns;
	std::vector<double> dReturns;
	int pickupType = 0;
	float pPosX = 0.0f;
	float pPosY = 0.0f;
	float pPosZ = 0.0f;

	Python->CallFunction( Python->GetFunction("getPickupType"), Python->CreateArg((int)0) );
	Python->Update(0.0f);
	if(Python->CheckReturns())
	{
		Python->ConvertInts(iReturns);
		Python->ClearReturns();
		if(iReturns.size() == 1)
		{
			pickupType = iReturns.at(0);
		}
		iReturns.clear();
	}


	Python->CallFunction( Python->GetFunction("getPickupPos"), Python->CreateArg((int)0) );
	Python->Update(0.0f);
	if(Python->CheckReturns())
	{
		Python->ConvertDoubles(dReturns);
		Python->ClearReturns();
		if(dReturns.size() > 0)
		{
			pPosX = (float)dReturns[0];
			pPosY = (float)dReturns[1];
			pPosZ = (float)dReturns[2];
		}
		dReturns.clear();
	}

	//std::cout << "x: " << pPosX << " y: " << pPosY << " z: " << pPosZ << std::endl;
	this->pickups.push_back( new Pickup(send, 0, pickupType, pPosX, pPosY, pPosZ, this->platforms.at(randomizedSpawnPos)->getID()) );
	this->nrOfPickups++;

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
	
	Python->LoadModule("pickup_script");

	for(unsigned int i = 0; i < pickups.size(); i++)
	{
		pickups[i]->Update(dt);
		Package* p = pickups[i]->GetDestroy();
		if(p != NULL)
		{
			Python->CallFunction( Python->GetFunction("consumePickup"), Python->CreateArg((int)0) ); //replace 0

			send->push(new PackageTo(p, 0));
			delete pickups[i];
			pickups.erase(pickups.begin()+i);
			break;
		}
	}

	for(int i = 0; i < nrOfPickups; i++)
	{
		Python->CallFunction(Python->GetFunction("checkActive"), Python->CreateArg((int)0) ); //should be incremental value here too
		Python->Update(dt);
		if(Python->CheckReturns())
		{
			std::vector<int> iReturns;
			Python->ConvertInts(iReturns);
			Python->ClearReturns();

			if(iReturns.size() == 1)
			{
				if(iReturns.at(0) == 1)
				{
					if( this->pickups.size() > 0)
					{
						this->pickups.at(i)->setRemove(false);
					}
					else
					{
						std::vector<int> iReturns2;
						std::vector<int> iReturns3;
						std::vector<double> dReturns;
						int pickupType = 0;
						int cpID = 0;
						float pPosX = 0.0f;
						float pPosY = 0.0f;
						float pPosZ = 0.0f;

						Python->CallFunction( Python->GetFunction("getPickupType"), Python->CreateArg((int)0) );
						Python->Update(dt);
						if(Python->CheckReturns())
						{
							Python->ConvertInts(iReturns2);
							Python->ClearReturns();
							if(iReturns2.size() == 1)
							{
								pickupType = iReturns2.at(0);
							}
							iReturns2.clear();
						}


						Python->CallFunction( Python->GetFunction("getPickupPos"), Python->CreateArg((int)0) );
						Python->Update(dt);
						if(Python->CheckReturns())
						{
							
							Python->ConvertDoubles(dReturns);
							Python->ClearReturns();
							if(dReturns.size() > 0)
							{
								pPosX = (float)dReturns[0];
								pPosY = (float)dReturns[1];
								pPosZ = (float)dReturns[2];
							}
							dReturns.clear();
						}

						Python->CallFunction( Python->GetFunction("getPlatID"), Python->CreateArg((int)0) );
						Python->Update(dt);
						if(Python->CheckReturns())
						{
							Python->ConvertInts(iReturns3);
							Python->ClearReturns();
							if(iReturns3.size() > 0)
							{
								cpID = iReturns3.at(0);
							}
							iReturns3.clear();
						}

						this->pickups.push_back( new Pickup(send, 0, pickupType, pPosX, pPosY, pPosZ, cpID) );

						for(unsigned int i = 0; i < this->pickups.size(); i++)
							send->push(new PackageTo(pickups[i]->GetConnect(), 0));
					}
				}
				else
				{
					if( this->pickups.size() > 0)
					{
						this->pickups.at(i)->setRemove(true);
					}
				}
			}
			//std::cout << "Returned value: " << iReturns.at(0) << std::endl;

			iReturns.clear();
		
		}

		Python->CallFunction( Python->GetFunction("update"), Python->CreateArg((int)0) );
		Python->Update(dt);
		//the update-call should be inside for-loop with and incrementing value instead of 0
		
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

				//respawn all players
				for(int i = 0; i < players->size(); i++)
				{
					Player* player = players->at(i);
					player->Reset();

					std::vector<double> dReturns;
					Python->LoadModule("player_script");
					Python->CallFunction(
						Python->GetFunction("GetRandomSpawn"),
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

					player->alive = true;
					player->health = 100;
					player->updated = true;
					send->push(new PackageTo(player->GetSpawn(), 0));
					Package* spawnP = player->GetSpawn();
					spawnP->SetId(0);
					send->push(new PackageTo(spawnP, (char*)player->GetId()));
					send->push(new PackageTo(player->GetSelfUpdate(), (char*)player->GetId()));
				}
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
			Python->GetFunction("GetRandomSpawn"),
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
		int id = *(int*)p->GetBody().Read(4);
		Player* player = findPlayer(players, id);
		if(player != NULL)
		{
			player->HandlePackage(p);
			send->push(new PackageTo(player->GetUpdate(), 0));
		}
		else
			send->push(new PackageTo(new Package(Package::Header(4, id, 0), Package::Body(new char())), socket)); //send disconnect
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

				Python->LoadModule("pickup_script");
				Python->CallFunction( Python->GetFunction("consumePickup"), Python->CreateArg((int)0) );
				
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