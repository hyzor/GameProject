#include "Platform.h"

Platform::Platform()
	: mEntity(0), mCollision(0), mFilePath("Data\\Models\\Collada\\")
{
}

Platform::~Platform()
{
	SafeDelete(mCollision);
	SafeDelete(mEntity);
}

void Platform::Initialize(int id, XMFLOAT3 pos)
{
	mFilePath += mModelName + "\\";
	this->mID = id;
	this->mEntity = new Entity(GenericHandler::GetInstance()->GetGenericModel(mModelName), pos);
	this->mCollision = new CollisionModel(pos);
	std::string finalPath = mFilePath + mModelName + ".obj";
	this->mCollision->LoadObj(finalPath);
}

void Platform::Move(XMFLOAT3 pos)
{
	mEntity->Position = pos;
	mCollision->SetPosition(pos);
}

void Platform::Update(float dt)
{
	HandleScript();
}

void Platform::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap)
{
	mEntity->Draw(dc, at, camera, shadowMap);
}
