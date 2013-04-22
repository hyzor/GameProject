#ifndef PLAYER_H_
#define PLAYER_H_

#define PI 3.1415926535897932384626433832795

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
	Player(GenericModel* model, int PlayerID, std::string Nickname, XMFLOAT3 Position);
	~Player();

	void Update(float dt, DirectInput* dInput, World* world);
	void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap);

	void SetPosition(XMFLOAT3 position) { mPosition=position; }
	int GetID() const { return mPlayerID; }
	XMFLOAT3 GetPosition() const { return mPosition; }
	Camera* GetCamera() { return mCamera; }
	bool IsAlive() const { return mIsAlive; }

	void TakeDamage(float damage);

private:
	int mPlayerID;
	float mHealth;

	float mSpeed;
	float ySpeed;

	bool mIsAlive;
	bool mInCameraFrustum;

	XMFLOAT3 mPosition;

	std::string mNickname;

	UINT mCurWeaponIndex;
	std::vector<Weapon*> mWeapons;

	Camera* mCamera;

	GenericModel* mModel;

	XMMATRIX* Joint;
	float rot;
	bool eDown;

	
	void Shoot();
};
#endif