#ifndef PLATFORM_H
#define PLATFORM_H

#include <string>
#include <queue>
#include "Package.h"
#include "PyEngine.h"

class Platform
{
	public:

		Platform(std::queue<PackageTo*>* send, int id, int type, float posX, float posY, float posZ);

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
		float movX;
		float movY;
		float movZ;
		std::vector<double> mdReturns;
		std::queue<PackageTo*>* send;
};
#endif