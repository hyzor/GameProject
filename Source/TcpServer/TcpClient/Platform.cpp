#include "Platform.h"
#include <iostream>

Platform::Platform(std::queue<PackageTo*>* send, int id, int type, float posX, float posY, float posZ)
{
	this->id = id;
	this->type = type;
	this->posX = posX;
	this->posY = posY;
	this->posZ = posZ;
	this->movX = 0;
	this->movY = 0;
	this->movZ = 0;

	update = false;
	std::cout << this->id << ": " << this->type << ", " << " ( " << this->posX << ", " << this->posY << ", " << this->posZ << " ) " << std::endl;
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
			float movX;
			float movY;
			float movZ;
		};

		PlatformUpdate* pu = new PlatformUpdate();
		pu->posX = posX;
		pu->posY = posY;
		pu->posZ = posZ;
		pu->movX = movX;
		pu->movY = movY;
		pu->movZ = movZ;

		update = false;

		return new Package(Package::Header(6, id, sizeof(PlatformUpdate)), Package::Body((char*)pu));
	}

	return NULL;
}

void Platform::Update(float dt)
{
	Python->LoadModule("platform_script");
	Python->CallFunction(
		Python->GetFunction("MovePlatform"),
		Python->CreateArg<int, double>(this->id, (double)dt));
	Python->Update(dt);
	if(Python->CheckReturns())
	{
		std::vector<double> dReturns;
		Python->ConvertDoubles(dReturns);
		Python->ClearReturns();
		int index = 0;
		if(!dReturns.empty())
		{
			this->posX = (float)dReturns[index];
			this->posY = (float)dReturns[index+1];
			this->posZ = (float)dReturns[index+2];
			this->movX = (float)dReturns[index+3];
			this->movY = (float)dReturns[index+4];
			this->movZ = (float)dReturns[index+5];

			this->update = true;
		}
	}
	//std::cout << this->id << ": " << this->type << ", " << " ( " << this->posX << ", " << this->posY << ", " << this->posZ << " ) " << std::endl;
}