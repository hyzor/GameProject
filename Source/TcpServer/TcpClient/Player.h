#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <queue>
#include "Package.h"
#include "PyEngine.h"
#include "GameTimer.h"
#include "Pickup.h"

class Player
{
	public:
		
		Player(std::queue<PackageTo*>* send, int id, std::string name);
		
		void HandlePackage(Package* p);
		void HandlePickup(Pickup* p);
		void Update();

		Package* GetConnect();
		Package* GetUpdate();
		Package* GetSelfUpdate();
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
		int score;

	private:
		int id;
		std::string name;

		std::queue<PackageTo*>* send;

		GameTimer mTimer;

};

#endif