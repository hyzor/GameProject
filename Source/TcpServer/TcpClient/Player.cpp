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

	Reset();

	this->mTimer.start();
	this->mTimer.reset();
}

void Player::Reset()
{
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
}

void Player::HandlePackage(Package* p)
{
	if(p->GetHeader().operation == 1)
	{
		Package::Body b = p->GetBody();
		float posX = *(float*)b.Read(4);
		float posY = *(float*)b.Read(4);
		float posZ = *(float*)b.Read(4);
		if(std::abs(posX-this->posX) < 100)
			this->posX = posX;
		if(std::abs(posY-this->posY) < 100)
			this->posY = posY;
		if(std::abs(posZ-this->posZ) < 100)
			this->posZ = posZ;

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
			Die();
			Player* enemy = findPlayer(players, p->GetHeader().id);
			if(enemy != NULL)
			{
				this->deathBy = enemy->GetId();
				enemy->kills++;
				enemy->updated = true;
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
	this->health = 200;
	this->updated = true;
}

void Player::Update()
{
	this->mRespawnTimer.tick();
	this->mTimer.tick();
	float dt = this->mTimer.getDeltaTime();


	if(this->alive)
	{
		//out of boundes
		if(posX > 1000 || posX < -1000 || posY > 1000 || posY < -1000 || posZ > 1000 || posZ < -1000)
			Die();
	}
	else
	{
		if(respawntime - mRespawnTimer.getTimeElapsedS() <= 0)
		{
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
				this->posX = (float)dReturns[0];
				this->posY = (float)dReturns[1];
				this->posZ = (float)dReturns[2];
			}

			this->alive = true;
			this->health = 100;
			this->updated = true;
			send->push(new PackageTo(this->GetSpawn(), 0));
			Package* spawnP = this->GetSpawn();
			spawnP->SetId(0);
			send->push(new PackageTo(spawnP, (char*)this->GetId()));
			send->push(new PackageTo(this->GetSelfUpdate(), (char*)this->GetId()));
		}
	}

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


void Player::Die()
{
	if(mRespawnTimer.getTimeElapsedS() > respawntime+3)
	{
		this->health = 0;
		this->deaths++;
		this->alive = false;
		updated = true;
		Python->LoadModule("respawn");
		Python->CallFunction(
			Python->GetFunction("PlayerDied"),
			nullptr);
		Python->Update(0.0f);

		std::vector<int> iReturns(0);
		if(Python->CheckReturns())
		{
			Python->ConvertInts(iReturns);
			Python->ClearReturns();
			respawntime = iReturns[0];
		}

		mRespawnTimer.start();
		mRespawnTimer.reset();
	}
}