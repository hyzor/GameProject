#include "Player.h"


Player::Player(void)
{
	mPlayerID = -1;
	mHealth = 1.0f;
	mIsAlive = false;

	mSpeed = 1.0f;

	mCurWeaponIndex = 0;
	mCamera = 0;
	mScale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	mPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mAngle = 0.0f;

	mInCameraFrustum = true;

	mNickname = "Player";
}

Player::Player(const Player& other)
{
}


Player::~Player(void)
{
	if (mCamera)
		SafeDelete(mCamera);

	for (UINT i = 0; i < mWeapons.size(); ++i)
		delete mWeapons[i];
}

bool Player::Shoot()
{
	if (mWeapons[mCurWeaponIndex]->GetCooldown() > 0.0f)
		return false;

	switch (mWeapons[mCurWeaponIndex]->GetType())
	{
	case Weapon::TYPE_RAILGUN:
		// Shoot laser beam
		// Emit new particles going forward in a straight line
		// Forming a type of beam
		mWeapons[mCurWeaponIndex]->FireProjectile(Weapon::TYPE_RAILGUN, mWeapons[mCurWeaponIndex]->GetNumProjectiles());
		break;
	}

	mWeapons[mCurWeaponIndex]->ResetCooldown();
	return true;
}

int Player::GetID() const
{
	return mPlayerID;
}

void Player::TakeDamage(float damage)
{
	mHealth -= damage;
}

bool Player::IsAlive() const
{
	return mIsAlive;
}

void Player::Update(float dt, DirectInput* dInput)
{
	// Invalid player ID, player is not yet alive
	if (mPlayerID < 0)
	{
		mIsAlive = false;
		return;
	}

	// Health lower than 0, die
	if (mHealth < 0.0f)
		mIsAlive = false;

	for (UINT i = 0; i < mWeapons.size(); ++i)
	{
		mWeapons[i]->Update(dt);
	}

	// Up
	if (dInput->GetKeyboardState()[DIK_W] && 0x80)
	{
		mCamera->walk(30.0f*dt);
	}

	// Left
	if (dInput->GetKeyboardState()[DIK_A] & 0x80)
	{
		mCamera->strafe(-30.0f*dt);
	}

	// Down
	if (dInput->GetKeyboardState()[DIK_S] & 0x80)
	{
		mCamera->walk(-30.0f*dt);
	}

	// Right
	if (dInput->GetKeyboardState()[DIK_D] & 0x80)
	{
		mCamera->strafe(30.0f*dt);
	}

	// Mouse has moved in x-axis
	if (dInput->MouseHasMoved())
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(dInput->GetMouseState().lX));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(dInput->GetMouseState().lY));

		mCamera->yaw(dx);
		mCamera->pitch(dy);
	}

	if (dInput->GetMouseState().rgbButtons[0])
	{
		Shoot();
	}	

	mCamera->updateViewMatrix();

	SetPosition(mCamera->getPosition());
}

bool Player::Init(InitProperties playerProperties)
{
	if (playerProperties.PlayerID > -1)
	{
		mHealth = playerProperties.Health;
		mNickname = playerProperties.Nickname;
		mSpeed = playerProperties.Speed;
		mPosition = playerProperties.Position;
		mScale = playerProperties.Scale;
		mAngle = playerProperties.Angle;
		mPlayerID = playerProperties.PlayerID;
		mIsAlive = true;

		if (!mCamera)
			mCamera = new Camera();

		mModelInstance = playerProperties.ModelInstance;

		SetPosition(mPosition);
		SetScale(mScale);

		//mModelInstance.isVisible = true;
		//mModelInstance.model = playerProperties.Model;

		// Weapons
		Weapon* weapon = new Weapon();
		Weapon::Properties prop;
		prop.Type = Weapon::TYPE_RAILGUN;
		prop.Damage = 1.0f;
		prop.Cooldown = 5.0f;
		prop.NumProjectiles = 1;
		weapon->Init(prop);
		mWeapons.push_back(weapon);
		mCurWeaponIndex = 0;

		return true;
	}

	return false;
}

void Player::SetPosition(XMFLOAT3 position)
{
	mPosition = position;

	XMMATRIX modelOffset = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
	XMMATRIX modelScale = XMMatrixScaling(mScale.x, mScale.y, mScale.z);
	XMMATRIX modelRotY = XMMatrixRotationY(mAngle);

	XMStoreFloat4x4(&mModelInstance.world, modelScale*modelRotY*modelOffset);
}

void Player::SetScale(XMFLOAT3 scale)
{
	mScale = scale;

	XMMATRIX modelOffset = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
	XMMATRIX modelScale = XMMatrixScaling(mScale.x, mScale.y, mScale.z);
	XMMATRIX modelRotY = XMMatrixRotationY(mAngle);

	XMStoreFloat4x4(&mModelInstance.world, modelScale*modelRotY*modelOffset);
}

XMFLOAT3 Player::GetPosition() const
{
	return mPosition;
}

Camera* Player::GetCamera()
{
	return mCamera;
}

void Player::Draw(ID3D11DeviceContext* dc,
	DirectionalLight lights[3],
	ID3D11ShaderResourceView* shadowMap,
	XMMATRIX* shadowTransform)
{
	//if (!mInCameraFrustum)
		//return;

	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dc->IASetInputLayout(InputLayouts::Basic32);

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;

	ID3DX11EffectTechnique* activeTech = Effects::BasicFX->DirLights3TexTech;

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	XMMATRIX view = mCamera->getViewMatrix();
	XMMATRIX proj = mCamera->getProjMatrix();
	XMMATRIX viewproj = mCamera->getViewProjMatrix();

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		world = XMLoadFloat4x4(&mModelInstance.world);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
		Effects::BasicFX->setShadowTransform(world*(*shadowTransform));
		Effects::BasicFX->setShadowMap(shadowMap);
		Effects::BasicFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));


		Effects::BasicTessFX->setFogColor(Colors::Silver);

		for (UINT i = 0; i < mModelInstance.model->meshCount; ++i)
		{
			UINT matIndex = mModelInstance.model->meshes[i].MaterialIndex;

			Effects::BasicTessFX->SetMaterial(mModelInstance.model->mat[matIndex]);

			Effects::BasicTessFX->SetDiffuseMap(mModelInstance.model->diffuseMapSRV[matIndex]);
			//Effects::BasicTessFX->SetNormalMap(mModelInstance.model->normalMapSRV[matIndex]);

			activeTech->GetPassByIndex(p)->Apply(0, dc);
			mModelInstance.model->meshes[i].draw(dc);
		}
	}

}
