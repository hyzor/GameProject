#ifndef PLAYERMULTI_H
#define PLAYERMULTI_H

#include "Player.h"
#include "Package.h"
#include "AnimatedEntity.h"

class PlayerMulti : public Player
{
	public:
		PlayerMulti(int PlayerID, std::string Nickname, XMFLOAT3 Position);
		~PlayerMulti();
		
		void Update(float dt, float gameTime, DirectInput* dInput,SoundModule* sm, World* world, std::vector<Player*>* multiplayers);
		void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap);
		
		void HandelPackage(Package *p);

	private:
		AnimatedEntity* mModel;
};


#endif