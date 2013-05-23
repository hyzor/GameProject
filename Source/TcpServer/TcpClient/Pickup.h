#ifndef PICKUP_H
#define PICKUP_H

#include <string>
#include <queue>
#include "Package.h"
#include "PyEngine.h"

class Pickup
{
public:
	Pickup(std::queue<PackageTo*>* send, int id, int type, float posX, float posY, float posZ);
	Package* GetConnect();
	Package* GetDestroy();
	void HandlePackage(Package* p);
	int GetId(){return id;}
	
	int type;
private:
	int id;
	float posX;
	float posY;
	float posZ;
	bool remove;
	std::vector<double> mdReturns;
	std::queue<PackageTo*>* send;
};
#endif