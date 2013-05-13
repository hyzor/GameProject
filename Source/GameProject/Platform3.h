#ifndef PLATFORM3_H
#define PLATFORM3_H
#include "Platform.h"

class Platform3 : public Platform
{
public:
	Platform3(void);
	~Platform3(void);
	void Initialize(int id, XMFLOAT3 pos);
	void Update(float dt);
	void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap);
	void HandleScript();
};
#endif