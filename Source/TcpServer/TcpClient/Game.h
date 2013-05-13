#ifndef GAME_H
#define GAME_H

#include <queue>
#include "Package.h"
#include "Player.h"

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
		std::queue<PackageTo*>* send;

		Player* findPlayer(int id);

};


#endif