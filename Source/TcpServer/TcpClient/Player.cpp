#include "Player.h"

Player::Player(std::queue<PackageTo*>* send, std::vector<Player*>* players, int id, std::string name)
{
	this->players = players;

	this->id = id;
	this->name = name;

	//std::cout << this->name << " @( " << this->posX << ", " << this->posY << ", " << this->posZ << " )" << std::endl;
	alive = true;

	updated = false;

	this->send = send;

	posX = 0;
	posY = 0;
	posZ = 0;
	movX = 0;
	movY = 0;
	movZ = 0;
	rotX = 0;
	rotY = 0;
	rotZ = 0;
	pitch = 0;
	roll = 0;
	yaw = 0;
	alive = 1;
	health = 100;
	kills = 0;
	deaths = 0;
	respawntime = 10;
	deathBy = 0;

	this->mTimer.start();
	this->mTimer.reset();
}

void Player::HandlePackage(Package* p)
{
	if(p->GetHeader().operation == 1)
	{
		Package::Body b = p->GetBody();
		this->posX = *(float*)b.Read(4);
		this->posY = *(float*)b.Read(4);
		this->posZ = *(float*)b.Read(4);
		this->movX = *(float*)b.Read(4);
		this->movY = *(float*)b.Read(4);
		this->movZ = *(float*)b.Read(4);
		this->rotX = *(float*)b.Read(4);
		this->rotY = *(float*)b.Read(4);
		this->rotZ = *(float*)b.Read(4);
		this->pitch = *(float*)b.Read(4);
		this->roll = *(float*)b.Read(4);
		this->yaw = *(float*)b.Read(4);
	}
	else if(p->GetHeader().operation == 11)
	{
		//player shoot
		this->health -= 100;
		if(this->health <= 0)
		{
			this->deaths++;
			this->alive = false;
			Player* enemy = findPlayer(players, p->GetHeader().id);
			if(enemy != NULL)
			{
				this->deathBy = enemy->GetId();
				enemy->kills++;
			}
			else
				this->deathBy = 0;
		}
		this->updated = true;
	}
}

void Player::HandlePickup(Pickup* p)
{
	//give pickup effect
	this->health += 100;
	this->updated = true;
}

void Player::Update()
{
	this->mTimer.tick();
	float dt = this->mTimer.getDeltaTime();


	if(this->alive)
	{
		//out of boundes
		if(posX > 1000 || posX < -1000 || posY > 1000 || posY < -1000 || posZ > 1000 || posZ < -1000)
		{
			this->health = 0;
			this->deaths++;
			this->alive = false;
			this->deathBy = 0;
			updated = true;
		}
	}

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
		float posX;
		float posY;
		float posZ;
		float health;
		int alive;
		char name[50];
	};

	PlayerConnect* pc = new PlayerConnect();
	pc->posX = posX;
	pc->posY = posY;
	pc->posZ = posZ;
	pc->health = health;
 	pc->alive = alive;
	for(int i = 0; i < name.length(); i++)
		pc->name[i] = name[i];
	pc->name[name.length()] = 0;

	return new Package(Package::Header(2, id, sizeof(PlayerConnect)), Package::Body((char*)pc));
}

Package* Player::GetUpdate()
{
	struct PlayerUpdate
	{
		float posX;
		float posY;
		float posZ;
		float movX;
		float movY;
		float movZ;
		float rotX;
		float rotY;
		float rotZ;
		float pitch;
		float roll;
		float yaw;
		int alive;
		float health;
		int kills;
		int deaths;
		int deathBy;
	};

	PlayerUpdate* pu = new PlayerUpdate();
	pu->posX = posX;
	pu->posY = posY;
	pu->posZ = posZ;
	pu->movX = movX;
	pu->movY = movY;
	pu->movZ = movZ;
	pu->rotX = rotX;
	pu->rotY = rotY;
	pu->rotZ = rotZ;
	pu->pitch = pitch;
	pu->roll = roll;
	pu->yaw = yaw;
	pu->alive = alive;
	pu->health = health;
	pu->kills = kills;
	pu->deaths = deaths;
	pu->deathBy = deathBy;

	return new Package(Package::Header(1, id, sizeof(PlayerUpdate)), Package::Body((char*)pu));
}

Package* Player::GetSelfUpdate()
{
	if(updated)
	{
		struct PlayerSelf
		{
			int alive;
			float health;
			int kills;
			int deaths;
			float respawntime;
			int deathBy;
		};

		PlayerSelf* ps = new PlayerSelf();
		ps->alive = this->alive;
		ps->health = this->health;
		ps->kills = this->kills;
		ps->deaths = this->deaths;
		ps->respawntime = this->respawntime;
		ps->deathBy = this->deathBy;

		this->updated = false;

		return new Package(Package::Header(10, 0, sizeof(PlayerSelf)), Package::Body((char*)ps));
	}
	return NULL;
}

Package* Player::GetSpawn()
{
	struct PlayerSpawn
	{
		float posX;
		float posY;
		float posZ;
		int alive;
		float health;
	};

	PlayerSpawn* ps = new PlayerSpawn();
	ps->posX = posX;
	ps->posY = posY;
	ps->posZ = posZ;
	ps->alive = alive;
	ps->health = health;

	return new Package(Package::Header(3, id, sizeof(PlayerSpawn)), Package::Body((char*)ps));
}

int Player::GetId()
{
	return id;
}


Player* findPlayer(std::vector<Player*>* players, int id)
{
	for(unsigned int i = 0; i < players->size(); i++)
		if(players->at(i)->GetId() == id)
			return players->at(i);
	return nullptr;
}