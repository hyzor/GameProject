#ifndef GAME_H
#define GAME_H

#include <d3dUtilities.h>
#include <D3D11App.h>
#include <Camera.h>
#include <TextureManager.h>

#include "GenericModel.h"
#include "Entity.h"
#include "Player.h"

#include "AnimatedEntity.h"
#include "GenericSkinnedModel.h"
#include "GenericHandler.h"

class Game
{
	public:
		Game(ID3D11Device* device, TextureManager* mTextureMgr);
		~Game();

		void Update(float deltaTime, DirectInput* di);
		void Draw(ID3D11DeviceContext* dc, ShadowMap* shadowMap);

		Camera* GetCamera();
	private:
		Entity* mDuck;
		CollisionModel* mDuckColl;

		Entity* mDuck2;
		CollisionModel* mDuckColl2;

		Player* mPlayer;

		AnimatedEntity* mAnimatedEntity;
		GenericSkinnedModel* mSkinnedModel;
};



#endif