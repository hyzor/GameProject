#ifndef ANIMATEDENTITY_H_
#define ANIMATEDENTITY_H_

#include "GenericSkinnedModel.h"
#include <Camera.h>
#include "ShadowMap.h"

class AnimatedEntity
{
public:
	AnimatedEntity(void);
	AnimatedEntity(GenericSkinnedModel* model, XMFLOAT3 position);
	~AnimatedEntity(void);

	void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap);
	void Update(float dt);

	void SetPosition(XMFLOAT3 pos);
	void RotateXYZ(XMFLOAT3 rot, float yaw, XMVECTOR Up);

	XMFLOAT3 Position;
	XMFLOAT3 Scale;
	float Rotation;

private:
	GenericSkinnedModelInstance mInstance;
};

#endif