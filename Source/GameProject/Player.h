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
		Player(SoundModule* sm, int PlayerID, std::string Nickname, XMFLOAT3 Position, std::vector<Player*>* multiplayers, int index);
		virtual ~Player();

		virtual void Update(float dt, float gameTime, DirectInput* dInput, World* world, std::vector<Player*>* multiplayers);
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
		int deathBy;

		int index;
		std::string mNickname;

		//-------------------------------------------
		// Animations
		//-------------------------------------------
		enum AnimationTypes
		{
			IdleAnim = 0,
			RunningAnim,
			JumpingAnim,
			StrafingRightAnim,
			StrafingLeftAnim
		};

		struct Animation
		{
			Animation(UINT animationType, UINT frameStart, UINT frameEnd) 
			{ 
				this->AnimationType = animationType;
				this->FrameStart = frameStart;
				this->FrameEnd = frameEnd;
			}

			UINT FrameStart, FrameEnd;
			UINT AnimationType;
		};

		std::vector<Animation> mAnimations;
		UINT mCurAnim;

	protected:
		int mPlayerID;
		float mSpeed;
		bool OnGround;
		bool rotating;
		float aliveTime;

		float ySpeed;
		XMFLOAT3 mPosition;
		XMFLOAT3 rotation;
		XMFLOAT3 move;
		XMFLOAT3 relativeMotion;
		XMFLOAT3 rotateTo;
		XMFLOAT3 removeDown;
		
		Camera* mCamera;
		XMFLOAT4X4 Joint;

		int mCurWeaponIndex;
		std::vector<Weapon*> mWeapons;

		SoundModule* sm;
		std::vector<Player*>* multiplayers;

		void setKillsDeaths(int kills, int deaths);

};
#endif