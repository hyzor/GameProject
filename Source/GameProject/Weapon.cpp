#include "Weapon.h"

Weapon::Weapon()
	: mEntity(0)
{
	// Default values
	mProperties.damage = 1.0f;
	mProperties.cooldown = 5.0f;
	mProperties.currCooldown = 0.0f;
	mProperties.maxProjectiles = 1;
	mProperties.currProjectiles = 0;
}

Weapon::~Weapon()
{
	SafeDelete(mEntity);
}

void Weapon::Init(Properties properties, std::string modelName, XMFLOAT3 pos)
{
	mProperties.cooldown = properties.cooldown;
	mProperties.currCooldown = 0;
	mProperties.maxProjectiles = properties.maxProjectiles;
	mProperties.damage = properties.damage;
	mProperties.currProjectiles = 0;

	mEntity = new Entity(GenericHandler::GetInstance()->GetGenericModel(modelName), pos);
}

void Weapon::Update(float dt, float gameTime)
{
	if (mProperties.currCooldown > 0.0f)
		mProperties.currCooldown -= dt;
}

void Weapon::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, Camera* camera, ShadowMap* shadowMap, bool isVisible)
{
	// Draw weapon model
	if (isVisible)
		mEntity->Draw(dc, tech, camera, shadowMap);
}

void Weapon::ResetCooldown()
{
	mProperties.currCooldown = mProperties.cooldown;
}

bool Weapon::FireProjectile(XMFLOAT3 pos, XMFLOAT3 dir, XMFLOAT3 hitPos)
{
	if (mProperties.currCooldown <= 0.0f)
	{
		// Shoot something

		// Set cooldown to initial value
		ResetCooldown();

		// Successfully fired projectile
		return true;
	}

	return false;
}

void Weapon::SetPosition(XMFLOAT3 pos)
{
	mEntity->SetPosition(pos);
}

void Weapon::SetPosition(float x, float y, float z)
{
	mEntity->SetPosition(XMFLOAT3(x, y, z));
}

void Weapon::RotateRollPitchYaw(float yaw, float pitch, float roll)
{
	mEntity->RotateRollPitchYaw(yaw, pitch, roll);
}

void Weapon::ViewMatrixRotation(const XMMATRIX& view )
{
	XMMATRIX modelRot = view;

	XMMATRIX A = view;
	A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR det = XMMatrixDeterminant(A);
	modelRot = XMMatrixInverse(&det, modelRot);

	XMMATRIX modelScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);

	XMMATRIX weaponOffset = XMMatrixTranslation(0.0f, -2.3f, 5.0f);

	// Rotate weapon 180 degrees (to face it forwards)
	XMVECTOR yawAxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX weaponRotOffset = XMMatrixRotationAxis(yawAxis, MathHelper::DegreesToRadians(180.0f));
	
	// Model is positioned and rotated according to the view-matrix with a position offset and rotation offset
	XMStoreFloat4x4(&mEntity->mInstance.world, weaponRotOffset*weaponOffset*modelRot);
}
