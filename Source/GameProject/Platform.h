#ifndef PLATFORM_H
#define PLATFORM_H
#include "GenericHandler.h"
#include "CollisionModel.h"
#include "Entity.h"
#include "PlatformSwitch.h"

/* Abstract base class for different platforms */

class Platform
{
public:
	Platform();
	virtual ~Platform();
	virtual void Initialize(int id, XMFLOAT3 pos);
	virtual void Update(float dt);
	virtual void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap);
	virtual void HandleScript() = 0;
	CollisionModel* getCollision() { return this->mCollision; }
	void Move(XMFLOAT3 pos);

protected:
	std::string mModelName;
	std::vector<PlatformSwitch*> mSwitches;
	int mID;
	std::string mFilePath;

private:
	CollisionModel* mCollision;
	Entity* mEntity;
};
#endif