#ifndef PLATFORM_H
#define PLATFORM_H

#include <string>
#include <queue>
#include "Package.h"

class Platform
{
	public:

		Platform(std::queue<PackageTo*>* send, int id, int type, float posX, float posY, float posZ);

		Package* GetConnect();
		Package* GetUpdate();

		void Update();


		bool update;

	private:

		int id;
		int type;
		float posX;
		float posY;
		float posZ;

		std::queue<PackageTo*>* send;
};




#endif