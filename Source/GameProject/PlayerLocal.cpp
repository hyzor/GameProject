#include "PlayerLocal.h"

PlayerLocal::PlayerLocal(std::string Nickname, XMFLOAT3 Position) : PlayerLocal::Player(0, Nickname, Position)
{
	//send connect package
	char* name = new char[50];
	for(int i = 0; i < Nickname.length(); i++)
		name[i] = Nickname[i];
	Network::GetInstance()->Push(new Package(Package::Header(2, 0, 50), Package::Body(name)));

	t = 0;
}

PlayerLocal::~PlayerLocal()
{
}

void PlayerLocal::Update(float dt, DirectInput* dInput, World* world)
{
	//send package
	t += dt;
	if(t > 1)
	{
		t = 0;
		Network::GetInstance()->Push(new Package(Package::Header(1, 1, sizeof(XMFLOAT3)), Package::Body((char*)(new XMFLOAT3(this->GetPosition())))));
	}

	XMMATRIX cJoint = *Joint;

	//Remove down movement
	XMVECTOR removeDown = XMLoadFloat3(&XMFLOAT3(0,1,0));
	removeDown = XMVector3Transform(removeDown, cJoint);
	if(XMVectorGetX(removeDown) < 0.5f)
		removeDown = XMVectorSetX(removeDown, 1);
	else
		removeDown = XMVectorSetX(removeDown, 0);
	if(XMVectorGetY(removeDown) < 0.5f)
		removeDown = XMVectorSetY(removeDown, 1);
	else
		removeDown = XMVectorSetY(removeDown, 0);
	if(XMVectorGetZ(removeDown) < 0.5f)
		removeDown = XMVectorSetZ(removeDown, 1);
	else
		removeDown = XMVectorSetZ(removeDown, 0);
	
	//move
	XMVECTOR m = XMLoadFloat3(&XMFLOAT3(0,0,0));
	XMVECTOR look = XMVector3Normalize(mCamera->GetLookXM()*removeDown);
	if (dInput->GetKeyboardState()[DIK_W] & 0x80)
		m += look*mSpeed*dt;
	if (dInput->GetKeyboardState()[DIK_S] & 0x80)
		m -= look*mSpeed*dt;
	XMVECTOR right = XMVector3Normalize(mCamera->GetRightXM()*removeDown);
	if (dInput->GetKeyboardState()[DIK_D] & 0x80)
		m += right*mSpeed*dt;
	if (dInput->GetKeyboardState()[DIK_A] & 0x80)
		m -= right*mSpeed*dt;
	XMStoreFloat3(&move, m);

	// Rotate camera
	if (dInput->MouseHasMoved())
	{
		float dx = XMConvertToRadians(0.25f*static_cast<float>(dInput->GetMouseState().lX));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(dInput->GetMouseState().lY));

		mCamera->Yaw += dx;;
		mCamera->Pitch += dy;
	}

	//jump
	if (this->OnGround && dInput->GetKeyboardState()[DIK_SPACE] & 0x80)
		ySpeed = -200;


	this->Player::Update(dt, dInput, world);
}

void PlayerLocal::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap)
{



	this->Player::Draw(dc, activeTech, mCamera, shadowMap);
}
	
void PlayerLocal::HandelPackage(Package *p)
{
}

