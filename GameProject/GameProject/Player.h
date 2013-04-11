#ifndef PLAYER_H_
#define PLAYER_H_

#include <d3dUtilities.h>
#include "Weapon.h"
#include "GenericSkinnedModel.h"
#include "GenericModel.h"
#include "Effects.h"
#include <DirectInput.h>
#include "CollisionModel.h"
#include "Entity.h"





class Player
{
public:

	Player(GenericModel* model, int PlayerID, std::string Nickname, XMFLOAT3 Position);
	~Player();


	void Update(float dt, DirectInput* dInput, CollisionModel* world);
	void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap);



	void SetPosition(XMFLOAT3 position) {mPosition=position;};

	int GetID() const {return mPlayerID;};
	XMFLOAT3 GetPosition() const {return mPosition;};
	Camera* GetCamera() {return mCamera;};
	bool IsAlive() const {return mIsAlive;};


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

	std::vector<Weapon*> mWeapons;
	UINT mCurWeaponIndex;

	Camera* mCamera;

	GenericModel* mModel;

	XMMATRIX Joint;
	bool eDown;

	
	void Shoot();
};

#endif