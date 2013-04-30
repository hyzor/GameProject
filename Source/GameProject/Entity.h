#ifndef ENTITY_H_
#define ENTITY_H_


#include <d3dUtilities.h>
#include <D3D11App.h>
#include <Camera.h>
#include <TextureManager.h>
#include <xnacollision.h>

#include "GenericModel.h"
#include "ShadowMap.h"

class Entity
{
public:
	Entity(void);
	Entity(GenericModel* Model, XMFLOAT3 Position);
	~Entity(void);

	void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap);

	XMFLOAT3 Position;
	XMFLOAT3 Scale;
	float Rotation;

private:
	GenericModelInstance mInstance;
};

#endif