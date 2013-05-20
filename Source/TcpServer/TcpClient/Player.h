#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <queue>
#include "Package.h"

class Player
{
	public:
		
		Player(std::queue<PackageTo*>* send, int id, std::string name);
		
		void HandelPackage(Package* p);
		void Update(float dt);

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

		std::queue<PackageTo*>* send;

};

#endif