#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <queue>
#include "Package.h"
#include "PyEngine.h"
#include "GameTimer.h"

class Player
{
	public:
		
		Player(std::queue<PackageTo*>* send, int id, std::string name);
		
		void HandelPackage(Package* p);
		void Update();

		Package* GetConnect();
		Package* GetUpdate();
		Package* GetSpawn();

		int GetId();


		bool updated;

		float posX;
		float posY;
		float posZ;
		float movX;
		float movY;
		float movZ;
		float rotX;
		float rotY;
		float rotZ;
		float pitch;
		float roll;
		float yaw;
		int alive;
		float health;


	private:
		int id;
		std::string name;

		std::queue<PackageTo*>* send;

		GameTimer mTimer;

};

#endif