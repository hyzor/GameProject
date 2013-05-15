#include "PlayerMulti.h"

PlayerMulti::PlayerMulti(int PlayerID, std::string Nickname, XMFLOAT3 Position) : PlayerMulti::Player(PlayerID, Nickname, Position)
{
	mModel = new AnimatedEntity(GenericHandler::GetInstance()->GetGenericSkinnedModel("SkinnedModel"), Position);
}

PlayerMulti::~PlayerMulti()
{
	delete mModel;
}

void PlayerMulti::Update(float dt, float gameTime, DirectInput* dInput, SoundModule* sm, World* world)
{
	mModel->Update(dt);

	

	this->Player::Update(dt, gameTime, dInput, sm, world);
	mModel->SetPosition(this->mPosition);
}

void PlayerMulti::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap)
{
	mModel->Draw(dc, activeTech, mCamera, shadowMap);

	this->Player::Draw(dc, activeTech, mCamera, shadowMap);
}

void PlayerMulti::HandelPackage(Package *p)
{
	if (p->GetHeader().operation == 1)
		mPosition = *(XMFLOAT3*)p->GetBody().Read(sizeof(XMFLOAT3));
}