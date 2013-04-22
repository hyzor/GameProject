#ifndef GAME_H
#define GAME_H

#include <d3dUtilities.h>
#include <D3D11App.h>
#include <Camera.h>
#include <TextureManager.h>
#include <vector>

#include "Entity.h"
#include "Player.h"

#include "AnimatedEntity.h"
#include "GenericHandler.h"

#include "World.h"

class Game
{
	public:
		Game(ID3D11Device* device, TextureManager* mTextureMgr);
		~Game();

		void Update(float deltaTime, DirectInput* di);
		void Draw(ID3D11DeviceContext* dc, ShadowMap* shadowMap);

		Camera* GetCamera();
	private:
		World* mWorld;
		Player* mPlayer;

		AnimatedEntity* mAnimatedEntity;
		GenericSkinnedModel* mSkinnedModel;
};
#endif