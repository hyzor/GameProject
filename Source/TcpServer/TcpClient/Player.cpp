#include "Player.h"


Player::Player(std::queue<PackageTo*>* send, int id, std::string name)
{
	this->id = id;
	this->name = name;

	alive = false;
	posX = 0;
	posY = 0;
	posZ = 0;

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

void Player::Update()
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