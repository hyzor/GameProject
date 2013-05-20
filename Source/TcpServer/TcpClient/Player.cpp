#include "Player.h"

Player::Player(std::queue<PackageTo*>* send, int id, std::string name)
{
	this->id = id;
	this->name = name;

	Python->LoadModule("player_script");
	Python->CallFunction(
		Python->GetFunction("CreatePlayer"),
		Python->CreateArg(this->id, this->name.c_str()));
	Python->Update(0.0f);
	if(Python->CheckReturns())
	{
		std::vector<double> dReturns;
		Python->ConvertDoubles(dReturns);
		Python->ClearReturns();
		int index = 0;
		for(unsigned int i(0); i < dReturns.size()/3; ++i)
		{
			this->posX = (float)dReturns[index];
			this->posY = (float)dReturns[index+1];
			this->posZ = (float)dReturns[index+2];
		}
	}
	//std::cout << this->name << " @( " << this->posX << ", " << this->posY << ", " << this->posZ << " )" << std::endl;
	alive = false;

	updated = false;

	this->send = send;
}

void Player::HandelPackage(Package* p)
{
	if(p->GetHeader().operation == 1)
	{
		Package::Body b = p->GetBody();
		this->posX = *(float*)b.Read(4);
		this->posY = *(float*)b.Read(4);
		this->posZ = *(float*)b.Read(4);
	}
}

void Player::Update(float dt)
{
	////respawn player if dead
	//posX = 0;
	//posY = 0;
	//posZ = 0;
	//alive = true;
	//send->push(new PackageTo(this->GetSpawn(), (char*)id));
}

Package* Player::GetConnect()
{
	struct PlayerConnect
	{
		char name[50];
	};

	PlayerConnect* pc = new PlayerConnect();
	for(int i = 0; i < 50; i++)
		pc->name[i] = name[i];

	return new Package(Package::Header(2, id, sizeof(PlayerConnect)), Package::Body((char*)pc));
}

Package* Player::GetUpdate()
{
	struct PlayerUpdate
	{
		float posX;
		float posY;
		float posZ;
		//fill   alive, rot, aim
	};

	PlayerUpdate* pu = new PlayerUpdate();
	pu->posX = posX;
	pu->posY = posY;
	pu->posZ = posZ;

	return new Package(Package::Header(1, id, sizeof(PlayerUpdate)), Package::Body((char*)pu));
}

Package* Player::GetSpawn()
{
	struct PlayerSpawn
	{
		bool alive;
		float posX;
		float posY;
		float posZ;
	};

	PlayerSpawn* ps = new PlayerSpawn();
	ps->alive = alive;
	ps->posX = posX;
	ps->posY = posY;
	ps->posZ = posZ;

	return new Package(Package::Header(3,id, sizeof(PlayerSpawn)), Package::Body((char*)ps));
}

int Player::GetId()
{
	return id;
}