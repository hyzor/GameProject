#include "PlatformSwitch.h"


PlatformSwitch::PlatformSwitch(void)
{
}

PlatformSwitch::~PlatformSwitch(void)
{
	SafeDelete(mCollision);
	SafeDelete(mEntity);
}

void PlatformSwitch::Initialize(XMFLOAT3 pos, int type, int rotType, XMFLOAT4X4 rot, XMFLOAT3 collisionOffset)
{
	XMMATRIX rotM = XMLoadFloat4x4(&rot);
	XMFLOAT3 colPos = pos;
	colPos.x += collisionOffset.x;
	colPos.y += collisionOffset.y;
	colPos.z += collisionOffset.z;
	switch (type)
	{
	case 1:
		mEntity = new Entity(GenericHandler::GetInstance()->GetGenericModel("Switch"), pos);
		mCollision = new CollisionModel(colPos);
		mCollision->LoadObj("Data\\Models\\Collada\\Switch.obj"/*, rotM*/);
		break;
	case 2:
		mEntity = new Entity(GenericHandler::GetInstance()->GetGenericModel("SwitchY"), pos);
		mCollision = new CollisionModel(colPos);
		mCollision->LoadObj("Data\\Models\\Collada\\Switch_rotY.obj"/*, rotM*/);
		break;
	case 3:
		mEntity = new Entity(GenericHandler::GetInstance()->GetGenericModel("SwitchZ"), pos);
		mCollision = new CollisionModel(colPos);
		mCollision->LoadObj("Data\\Models\\Collada\\Switch_rotZ.obj"/*, rotM*/);
		break;
	}

	this->rotationType = rotType;
	
}

void PlatformSwitch::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap)
{
	mEntity->Draw(dc, at, camera, shadowMap);
}

Entity* PlatformSwitch::getEntity() const
{
	return this->mEntity;
}

XMVECTOR PlatformSwitch::GetMoveTo(XMVECTOR up)
{
	
	if(this->rotationType == 1)
	{
		if(XMVectorGetY(up) > 0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(0,-7,-7));
		else if(XMVectorGetZ(up) < -0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(0, 7, 7));
	}
	else if(this->rotationType == 2)
	{
		if(XMVectorGetY(up) > 0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(0,-7, 7));
		else if(XMVectorGetZ(up) > 0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(0, 7, -7));
	}
	else if(this->rotationType == 3)
	{
		if(XMVectorGetZ(up) > 0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(0, -7, -7));
		else if(XMVectorGetY(up) < -0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(0, 7, 7));
	}
	else if(this->rotationType == 4)
	{
		if(XMVectorGetZ(up) < -0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(0, -7, 7));
		else if(XMVectorGetY(up) < -0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(0, 7, -7));
	}
	else if(this->rotationType == 5)
	{
		if(XMVectorGetY(up) > 0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(15, -15, 0));
		else if(XMVectorGetX(up) > 0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(-15, 15, 0));
	}
	else if(this->rotationType == 6)
	{
		if(XMVectorGetY(up) > 0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(-15, -15, 0));
		else if(XMVectorGetX(up) < -0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(15, 15, 0));
	}
	else if(this->rotationType == 7)
	{
		if(XMVectorGetY(up) < -0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(-15, 15, 0));
		else if(XMVectorGetX(up) < -0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(15, -15, 0));
	}
	else if(this->rotationType == 8)
	{
		if(XMVectorGetZ(up) < -0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(-15, 0, 15));
		else if(XMVectorGetX(up) < -0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(15, 0, -15));
	}
	else if(this->rotationType == 9)
	{
		if(XMVectorGetZ(up) > 0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(15, 0, -15));
		else if(XMVectorGetX(up) > 0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(-15, 0, 15));
	}
	else if(this->rotationType == 10)
	{
		if(XMVectorGetZ(up) > 0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(-15, 0, -15));
		else if(XMVectorGetX(up) < -0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(15, 0, 15));
	}
	else if(this->rotationType == 11)
	{
		if(XMVectorGetZ(up) < -0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(15, 0, 15));
		else if(XMVectorGetX(up) > 0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(-15, 0, -15));
	}
	else if(this->rotationType == 12)
	{
		if(XMVectorGetY(up) < -0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(15, 15, 0));
		else if(XMVectorGetX(up) > 0.5f)
			return XMLoadFloat3(&mEntity->Position) + XMLoadFloat3(&XMFLOAT3(-15, -15, 0));
	}

	return XMLoadFloat3(&XMFLOAT3(0,0,0));
}

float PlatformSwitch::GetRotationX(XMVECTOR up)
{
	if(this->rotationType == 1)
	{
		if(XMVectorGetY(up) > 0.5f)
			return -(float)XM_PI/2;
		else if(XMVectorGetZ(up) < 0.5f)
			return +(float)XM_PI/2;
	}
	else if(this->rotationType == 2)
	{
		if(XMVectorGetY(up) > 0.5f)
			return +(float)XM_PI/2;
		else if(XMVectorGetZ(up) > 0.5f)
			return -(float)XM_PI/2;
	}
	else if(this->rotationType == 3)
	{
		if(XMVectorGetZ(up) > 0.5f)
			return +(float)XM_PI/2;
		else if(XMVectorGetY(up) < -0.5f)
			return -(float)XM_PI/2;
	}
	else if(this->rotationType == 4)
	{
		if(XMVectorGetZ(up) < -0.5f)
			return -(float)XM_PI/2;
		else if(XMVectorGetY(up) < 0.5f)
			return +(float)XM_PI/2;
	}

	return 0;
}
float PlatformSwitch::GetRotationZ(XMVECTOR up)
{
	if(this->rotationType == 5)
	{
		if(XMVectorGetY(up) > 0.5f)
			return -(float)XM_PI/2;
		else if(XMVectorGetX(up) > 0.5f)
			return +(float)XM_PI/2;
	}
	else if(this->rotationType == 6)
	{
		if(XMVectorGetY(up) > 0.5f)
			return +(float)XM_PI/2;
		else if(XMVectorGetX(up) < -0.5f)
			return -(float)XM_PI/2;
	}
	else if(this->rotationType == 7)
	{
		if(XMVectorGetY(up) < -0.5f)
			return -(float)XM_PI/2;
		else if(XMVectorGetX(up) < -0.5f)
			return +(float)XM_PI/2;
	}
	else if(this->rotationType == 12)
	{
		if(XMVectorGetY(up) < -0.5f)
			return +(float)XM_PI/2;
		else if(XMVectorGetX(up) > 0.5f)
			return -(float)XM_PI/2;
	}


	return 0;
}

float PlatformSwitch::GetRotationY(XMVECTOR up)
{
	if(this->rotationType == 8)
	{
		if(XMVectorGetZ(up) < -0.5f)
			return +(float)XM_PI/2;
		else if(XMVectorGetX(up) < -0.5f)
			return -(float)XM_PI/2;
	}
	else if(this->rotationType == 9)
	{
		if(XMVectorGetZ(up) > 0.5f)
			return +(float)XM_PI/2;
		else if(XMVectorGetX(up) > 0.5f)
			return -(float)XM_PI/2;
	}
	else if(this->rotationType == 10)
	{
		if(XMVectorGetZ(up) > 0.5f)
			return -(float)XM_PI/2;
		else if(XMVectorGetX(up) < -0.5f)
			return +(float)XM_PI/2;
	}
	else if(this->rotationType == 11)
	{
		if(XMVectorGetZ(up) < -0.5f)
			return -(float)XM_PI/2;
		else if(XMVectorGetX(up) > 0.5f)
			return +(float)XM_PI/2;
	}


	return 0;
}