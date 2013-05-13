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

class Player
{
	public:
		Player(int PlayerID, std::string Nickname, XMFLOAT3 Position);
		virtual ~Player();

		virtual void Update(float dt, DirectInput* dInput, World* world);
		virtual void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap);

		virtual void HandelPackage(Package *p);

		int GetID() const { return mPlayerID; }
		XMFLOAT3 GetPosition() const { return mPosition; }
		void SetPosition(XMFLOAT3 position) { mPosition=position; }
		Camera* GetCamera() { return mCamera; }
		bool IsAlive() const { return mIsAlive; }

		void TakeDamage(float damage);

	protected:
		int mPlayerID;
		float mHealth;
		std::string mNickname;
		float mSpeed;
		bool mIsAlive;
		XMFLOAT3 move;
		bool OnGround;

		float ySpeed;
		XMFLOAT3 mPosition;
		
		Camera* mCamera;
		XMMATRIX* Joint;
		float rot;
		bool eDown;

		int mCurWeaponIndex;
		std::vector<Weapon*> mWeapons;

		void Shoot();

	private:




	
};
#endif