#ifndef PLAYER_H_
#define PLAYER_H_

#include <d3dUtilities.h>
#include <Camera.h>
#include "Weapon.h"
#include "GenericSkinnedModel.h"
#include "GenericModel.h"
#include "Effects.h"
#include "GameSettings.h"
#include <DirectInput.h>

class Player
{
public:

	Player(void);
	Player(const Player& other);
	~Player(void);

	struct InitProperties
	{
		int PlayerID;
		std::string Nickname;
		float Health;
		float Speed;

		GenericModel* Model;
		XMFLOAT3 Scale;
		XMFLOAT3 Position;
		float Angle;
	};

	void Update(float dt, DirectInput* dInput);
	void Draw(ID3D11DeviceContext* dc,
		DirectionalLight lights[3],
		ID3D11ShaderResourceView* shadowMap,
		XMMATRIX* shadowTransform);

	bool Init(InitProperties playerProperties);

	void SetPosition(XMFLOAT3 position);

	int GetID() const;
	XMFLOAT3 GetPosition() const;
	Camera* GetCamera() const;

	bool Shoot();

	void TakeDamage(float damage);
	bool IsAlive() const;

private:
	int mPlayerID;
	float mHealth;

	float mSpeed;

	bool mIsAlive;
	bool mInCameraFrustum;

	XMFLOAT3 mPosition;
	XMFLOAT3 mScale;
	float mAngle;

	std::string mNickname;

	std::vector<Weapon*> mWeapons;
	UINT mCurWeaponIndex;

	Camera* mCamera;

	GenericModelInstance mModelInstance;
};

#endif