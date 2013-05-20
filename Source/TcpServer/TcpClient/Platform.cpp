#include "Platform.h"
#include <iostream>


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

void Platform::Update(float dt)
{
	this->update = true;
	Python->LoadModule("platform_script");
	Python->CallFunction(
		Python->GetFunction("MovePlatform"),
		Python->CreateArg(this->id));
	Python->Update(dt);
	if(Python->CheckReturns())
	{
		Python->ConvertDoubles(mdReturns);
		Python->ClearReturns();
		int index = 0;
		for(unsigned int i(0); i < mdReturns.size()/3; ++i)
		{
			this->posX = (float)mdReturns[index];
			this->posY = (float)mdReturns[index+1];
			this->posZ = (float)mdReturns[index+2];
			index += 3;
		}
		mdReturns.clear();
	}
	//std::cout << this->id << ": " << this->type << ", " << " ( " << this->posX << ", " << this->posY << ", " << this->posZ << " ) " << std::endl;
}