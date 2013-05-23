#include "Pickup.h"


Pickup::Pickup(std::queue<PackageTo*>* send, int id, int type, float posX, float posY, float posZ)
{
	this->id = id;
	this->type = type;

	this->posX = posX;
	this->posY = posY;
	this->posZ = posZ;

	this->send = send;

	this->update = false;
}

Package* Pickup::GetConnect()
{
	struct PickupConnect
	{
		int type;
		float posX;
		float posY;
		float posZ;
	};

	PickupConnect* puc = new PickupConnect();
	puc->type = this->type;
	puc->posX = this->posX;
	puc->posY = this->posY;
	puc->posZ = this->posZ;

	return new Package(Package::Header(7, this->id, sizeof(PickupConnect)), Package::Body((char*)puc));
}

Package* Pickup::GetUpdate()
{
	struct PickupUpdate
	{
		float posX;
		float posY;
		float posZ;
	};
	
	PickupUpdate* puu = new PickupUpdate();
	puu->posX = this->posX;
	puu->posY = this->posY;
	puu->posZ = this->posZ;

	return new Package(Package::Header(8, this->id, sizeof(PickupUpdate)), Package::Body((char*)puu));
}

void Pickup::Update(float dt)
{
	// this->update = true;
}
