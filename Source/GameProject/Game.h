#ifndef GAME_H
#define GAME_H

#include "Network.h"
#include <d3dUtilities.h>
#include <D3D11App.h>
#include <Camera.h>
#include <TextureManager.h>
#include <vector>
#include <typeinfo>

#include "Entity.h"
#include "GUI.h"
#include "PlayerLocal.h"
#include "PlayerMulti.h"

#include "AnimatedEntity.h"
#include "GenericHandler.h"

#include "World.h"
#include "Package.h"

#include "SoundModule.h"
#include "SoundHelp.h"

#include "Pickup.h"

class Game
{
	public:
		Game(ID3D11Device* device, ID3D11DeviceContext* dc, TextureManager* mTextureMgr, SoundModule* sm);
		~Game();

		void Update(float deltaTime, float gameTime, DirectInput* di);
		void Draw(ID3D11DeviceContext* dc, ShadowMap* shadowMap);
		void HandlePackage(Package* p);

		Player* GetPlayer() const;
		Camera* GetCamera();
	private:
		World* world;
		Player* player;
		std::vector<Player*>* multiplayers;

		int timeLeft;
		int pauseTimeLeft;
		bool gameActive;

		SoundModule* sm;

		std::vector<Pickup*> pickups;

		AnimatedEntity* animatedEntity;

		ID3D11Device* device; ID3D11DeviceContext* dc;

		void ResetGame();
};
#endif