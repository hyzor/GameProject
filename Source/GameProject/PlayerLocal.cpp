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
	rotateToX = 0;
	rotX = 0;
	rotateToZ = 0;
	rotZ = 0;
}

PlayerLocal::~PlayerLocal()
{
}

void PlayerLocal::Update(float dt, DirectInput* dInput, SoundModule* sm, World* world)
{
	
	XMVECTOR pos = XMLoadFloat3(&mPosition);

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
		//gravity switch
		XMVECTOR dir = XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,-1,0)), cJoint);
		PlatformSwitch* psCol = world->IntersectSwitch(pos+XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,10,0)), cJoint), dir, 10);
		if(psCol != NULL)
		{
			XMVECTOR up = XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0,1,0)), cJoint);
			float rX = psCol->GetRotationX(up);
			float rZ = psCol->GetRotationZ(up);

			if(!(rX == 0 && rZ == 0))
			{
				rotateToX += rX;
				rotateToZ += rZ;
				ySpeed = 0;
				rotating = true;
				pos = psCol->GetMoveTo(up);
			}
		}

		//switch garvity
		if(dInput->GetKeyboardState()[DIK_E] & 0x80)
		{
			if(!eDown)
			{
				rotateToX += (float)PI/2;
				//if(rotateToX > 2*(float)PI-0.1f)
					//rotateToX = 0;
				ySpeed = 0;
				rotating = true;
			}
			eDown = true;
		}
		else
			eDown = false;
		if(dInput->GetKeyboardState()[DIK_R] & 0x80)
		{
			if(!rDown)
			{
				rotateToZ += (float)PI/2*3;
				if(rotateToZ > 2*(float)PI-0.1f)
					rotateToZ = 0;
				ySpeed = 0;
				rotating = true;
			}
			rDown = true;
		}
		else
			rDown = false;

		//Remove down movement
		XMVECTOR removeDown = XMLoadFloat3(&XMFLOAT3(0,1,0));
		removeDown = XMVector3Transform(removeDown, cJoint);
		if(std::abs(XMVectorGetX(removeDown)) < 0.5f)
			removeDown = XMVectorSetX(removeDown, 1);
		else
			removeDown = XMVectorSetX(removeDown, 0);
		if(std::abs(XMVectorGetY(removeDown)) < 0.5f)
			removeDown = XMVectorSetY(removeDown, 1);
		else
			removeDown = XMVectorSetY(removeDown, 0);
		if(std::abs(XMVectorGetZ(removeDown)) < 0.5f)
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
		//rotating animation
		if(rotateToX != rotX)
		{
			float rt = rotateToX;
			float r = rotX;

			if(rt > (float)PI)
				rt = -((float)PI*2-rt);
			if(r > (float)PI)
				r = -((float)PI*2-r);

			if(rotateToX > rotX)
			{
				rotX += 5*dt;
				r += 5*dt;
				if(rt < r)
				{
					rotX = rotateToX;
					rotating = false;
				}
			}
			else
			{
				rotX -= 5*dt;
				r -= 5*dt;
				if(rt > r)
				{
					rotX = rotateToX;
					rotating = false;
				}
			}

			cJoint = XMMatrixRotationX(rotX);
			XMVECTOR look = XMLoadFloat3(&XMFLOAT3(0,0,1));
			look = XMVector3Transform(look, cJoint);
			cJoint = XMMatrixMultiply(cJoint, XMMatrixRotationAxis(look, rotZ));
		}
		else if(rotateToZ != rotZ)
		{
			float rt = rotateToZ;
			float r = rotZ;

			if(rt > (float)PI)
				rt = -((float)PI*2-rt);
			if(r > (float)PI)
				r = -((float)PI*2-r);

			if(rotateToZ > rotZ)
			{
				rotZ += 5*dt;
				r += 5*dt;
				if(rt < r)
				{
					rotZ = rotateToZ;
					rotating = false;
				}
			}
			else
			{
				rotZ -= 5*dt;
				r -= 5*dt;
				if(rt > r)
				{
					rotZ = rotateToZ;
					rotating = false;
				}
			}

			cJoint = XMMatrixRotationX(rotX);
			XMVECTOR look = XMLoadFloat3(&XMFLOAT3(0,0,1));
			look = XMVector3Transform(look, cJoint);
			cJoint = XMMatrixMultiply(cJoint, XMMatrixRotationAxis(look, rotZ));
		}
		else
			rotating = false;
	}

	delete Joint;
	this->Joint = new XMMATRIX(cJoint);
	XMStoreFloat3(&move, m);
	XMStoreFloat3(&mPosition, pos);

	this->Player::Update(dt, dInput, sm, world);
}

void PlayerLocal::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap)
{



	this->Player::Draw(dc, activeTech, mCamera, shadowMap);
}
	
void PlayerLocal::HandelPackage(Package *p)
{
}

