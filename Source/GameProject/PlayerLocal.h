#ifndef PLAYERLOCAL_H
#define PLAYERLOCAL_H

#include "Player.h"
#include "Package.h"

class PlayerLocal : public Player
{
	public:
		PlayerLocal(std::string Nickname, XMFLOAT3 Position);
		~PlayerLocal();

		void Update(float dt, float gameTime, DirectInput* dInput, SoundModule* sm, World* world);
		void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap);
		void Spawn(float x, float y, float z, int rotation);
		void HandelPackage(Package *p);
	private:
		float t;
		XMFLOAT3 rotateTo;

		bool eDown;
		bool rDown;
		bool tDown;

		int TimeToSpawn;
};


#endif