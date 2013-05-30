#ifndef PLAYER_H_
#define PLAYER_H_

#define PI 3.1415926535897932384626433832795

#include "Network.h"
#include <d3dUtilities.h>
#include "Weapon.h"
#include "GenericSkinnedModel.h"
#include "GenericModel.h"
#include "Effects.h"
#include <DirectInput.h>
#include "CollisionModel.h"
#include "Entity.h"
#include "Railgun.h"
#include "World.h"
#include "SoundModule.h"

class Weapon;

class Player
{
	public:
		Player(int PlayerID, std::string Nickname, XMFLOAT3 Position, int index);
		virtual ~Player();

		virtual void Update(float dt, float gameTime, DirectInput* dInput, SoundModule* sm, World* world, std::vector<Player*>* multiplayers);
		virtual void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap);

		virtual void HandelPackage(Package *p);

		void InitWeapons(ID3D11Device* device, ID3D11DeviceContext* dc);

		int GetID() const { return mPlayerID; }
		XMFLOAT3 GetPosition() const { return mPosition; }
		void SetPosition(XMFLOAT3 position) { mPosition=position; }
		Camera* GetCamera() { return mCamera; }
		bool IsAlive() const { return mIsAlive; }
		XNA::AxisAlignedBox GetBounding();


		bool mIsAlive;
		float mHealth;
		int kills;
		int deaths;
		float respawntime;

		int index;

	protected:
		int mPlayerID;
		std::string mNickname;
		float mSpeed;
		bool OnGround;
		bool rotating;
		float aliveTime;

		float ySpeed;
		XMFLOAT3 mPosition;
		XMFLOAT3 rotation;
		XMFLOAT3 move;
		XMFLOAT3 relativeMotion;
		
		Camera* mCamera;
		XMFLOAT4X4 Joint;

		int mCurWeaponIndex;
		std::vector<Weapon*> mWeapons;

};
#endif