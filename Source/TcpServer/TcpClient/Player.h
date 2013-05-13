#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include "Package.h"

class Player
{
	public:
		
		Player(int id, std::string name);
		
		void HandelPackage(Package* p);

		Package* GetConnect();
		Package* GetUpdate();
		Package* GetSpawn();

		int GetId();


		bool updated;
		bool alive;
		
		float posX;
		float posY;
		float posZ;
	private:
		int id;
		std::string name;


};

#endif