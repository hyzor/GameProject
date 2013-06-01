#ifndef PICKUP_H
#define PICKUP_H

#include <string>
#include <queue>
#include "Package.h"
#include "PyEngine.h"

class Pickup
{
public:
	Pickup(std::queue<PackageTo*>* send, int id, int type, float posX, float posY, float posZ, int cpID);
	Package* GetConnect();
	Package* GetDestroy();
	void HandlePackage(Package* p);
	void Update(float dt);
	int GetId(){return id;}
	void setRemove(bool flag) { this->remove = flag;}
	bool getRemove() const { return this->remove; }
	int getCPID() const { return this->connectedPlatformID;}
	
	int type;
private:
	int id;
	int connectedPlatformID;
	float posX;
	float posY;
	float posZ;
	bool remove;
	std::vector<double> mdReturns;
	std::queue<PackageTo*>* send;
};
#endif