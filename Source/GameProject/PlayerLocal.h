#ifndef PLAYERLOCAL_H
#define PLAYERLOCAL_H

#include "Player.h"
#include "Package.h"

class PlayerLocal : public Player
{
	public:
		PlayerLocal(SoundModule* sm, std::string Nickname, XMFLOAT3 Position, std::vector<Player*>* multiplayers);
		~PlayerLocal();

		void Update(float dt, float gameTime, DirectInput* dInput, World* world, std::vector<Player*>* multiplayers);
		void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap);
		void HandelPackage(Package *p);
	private:
		float t;

		bool eDown;
		bool rDown;
		bool tDown;

};


#endif