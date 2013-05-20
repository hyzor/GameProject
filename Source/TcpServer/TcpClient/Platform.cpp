#include "Platform.h"



Platform::Platform(std::queue<PackageTo*>* send, int id, int type, float posX, float posY, float posZ)
{
	this->id = id;
	this->type = type;
	this->posX = posX;
	this->posY = posY;
	this->posZ = posZ;

	update = false;

	this->send = send;
}

Package* Platform::GetConnect()
{
	struct PlatformConnect
	{
		int type;
		float posX;
		float posY;
		float posZ;
	};

	PlatformConnect* pc = new PlatformConnect();
	pc->type = type;
	pc->posX = posX;
	pc->posY = posY;
	pc->posZ = posZ;

	return new Package(Package::Header(5, id, sizeof(PlatformConnect)), Package::Body((char*)pc));
}

Package* Platform::GetUpdate()
{
	if(update)
	{
		struct PlatformUpdate
		{
			float posX;
			float posY;
			float posZ;
		};

		PlatformUpdate* pu = new PlatformUpdate();
		pu->posX = posX;
		pu->posY = posY;
		pu->posZ = posZ;

		update = false;

		return new Package(Package::Header(6, id, sizeof(PlatformUpdate)), Package::Body((char*)pu));
	}

	return NULL;
}

void Platform::Update()
{
	//move platform update=true
}