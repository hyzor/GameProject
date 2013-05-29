#include "Pickup.h"


Pickup::Pickup(std::queue<PackageTo*>* send, int id, int type, float posX, float posY, float posZ)
{
	this->id = id;
	this->type = type;

	this->posX = posX;
	this->posY = posY;
	this->posZ = posZ;

	this->send = send;

	this->remove = false;
}

void Pickup::Update(float dt)
{
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

Package* Pickup::GetDestroy()
{
	if(remove)
	{
		struct PickupDestroy
		{
			int id; //dummy
		};
	
		PickupDestroy* puu = new PickupDestroy();
		puu->id = this->id;

		return new Package(Package::Header(8, this->id, sizeof(PickupDestroy)), Package::Body((char*)puu));
	}
	return NULL;
}

void Pickup::HandlePackage(Package* p)
{
	if(p->GetHeader().operation == 9)
	{
		this->remove = true;
	}
}

