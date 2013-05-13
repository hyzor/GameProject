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

#include "PlayerLocal.h"
#include "PlayerMulti.h"

#include "AnimatedEntity.h"
#include "GenericHandler.h"

#include "World.h"
#include "Package.h"

#include "SoundModule.h"
#include "SoundHelp.h"

class Game
{
	public:
		Game(ID3D11Device* device, TextureManager* mTextureMgr);
		~Game();

		void Update(float deltaTime, DirectInput* di);
		void Draw(ID3D11DeviceContext* dc, ShadowMap* shadowMap);
		void HandlePackage(Package* p);
		void initSoundModule(HWND hwnd, DirectInput* di);

		Camera* GetCamera();
	private:
		World* world;
		Player* player;
		std::vector<Player*>* multiplayers;

		AnimatedEntity* animatedEntity;

		SoundModule* soundModule;

};
#endif