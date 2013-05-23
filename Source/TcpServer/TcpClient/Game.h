#ifndef GAME_H
#define GAME_H

#include <queue>
#include "Package.h"
#include "Player.h"
#include "Platform.h"
#include "PyEngine.h"
#include "GameTimer.h"
#include "Pickup.h"

class Game
{
	public:
		Game(std::queue<PackageTo*>* send);
		~Game();

		void Update();
		void HandelPackage(Package* p, char* socket);

		void Disconnect(char* socket);

	private:
		std::vector<Player*> players;
		std::vector<Platform*> platforms;
		std::vector<Pickup*> pickups;
		GameTimer mTimer;

		std::queue<PackageTo*>* send;

		Player* findPlayer(int id);

		void CheckPickups();

		float t;
};


#endif