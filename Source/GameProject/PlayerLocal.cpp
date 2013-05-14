#include "PlayerLocal.h"
#include <math.h>

PlayerLocal::PlayerLocal(std::string Nickname, XMFLOAT3 Position) : PlayerLocal::Player(0, Nickname, Position)
{
	//send connect package
	char* name = new char[50];
	for(UINT i = 0; i < Nickname.length(); i++)
		name[i] = Nickname[i];
	Network::GetInstance()->Push(new Package(Package::Header(2, 0, 50), Package::Body(name)));

	t = 0;
	rotateTo = 0;
	rot = 0;
}

PlayerLocal::~PlayerLocal()
{
}

void PlayerLocal::Update(float dt, DirectInput* dInput, SoundModule* sm, World* world)
{
	//send package
	t += dt;
	if(t > 1)
	{
		t = 0;
		Network::GetInstance()->Push(new Package(Package::Header(1, 1, sizeof(XMFLOAT3)), Package::Body((char*)(new XMFLOAT3(this->GetPosition())))));
	}

	XMMATRIX cJoint = *Joint;
	XMVECTOR m = XMLoadFloat3(&XMFLOAT3(0,0,0));

	if(!rotating)
	{
		//switch garvity
		if(dInput->GetKeyboardState()[DIK_E] & 0x80)
		{
			if(!eDown)
			{
				rotateTo += (float)PI/2;
				if(rotateTo > 2*(float)PI-0.1f)
					rotateTo = 0;
				ySpeed = 0;
				rotating = true;
			}
			eDown = true;
		}
		else
			eDown = false;

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
	}
	else
	{
		float rt = rotateTo;
		float r = rot;

		if(std::abs(rt-rot) > (float)PI)
		{
			if(rt < rot) rt += (float)PI*2;
			else rt -= (float)PI*2;
		}

		if(rt < r)
		{
			rot -= 0.0005f*dt;
			r -= 0.0005f*dt;
			if(rt > r)
			{
				rot = rotateTo;
				rotating = false;
			}
		}
		else
		{
			rot += 5*dt;
			r += 5*dt;
			if(rt < r)
			{
				rot = rotateTo;
				rotating = false;
			}
		}

		cJoint = XMMatrixRotationX(rot);
	}

	delete Joint;
	this->Joint = new XMMATRIX(cJoint);

	XMStoreFloat3(&move, m);
	this->Player::Update(dt, dInput, sm, world);
}

void PlayerLocal::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap)
{



	this->Player::Draw(dc, activeTech, mCamera, shadowMap);
}
	
void PlayerLocal::HandelPackage(Package *p)
{
}

