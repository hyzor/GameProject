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
	Package* GetUpdate();
	void Update(float dt);

	bool update;

private:
	int id;
	int type;
	float posX;
	float posY;
	float posZ;
	std::vector<double> mdReturns;
	std::queue<PackageTo*>* send;
};
#endif