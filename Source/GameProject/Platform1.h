#ifndef PLATFORM1_H
#define PLATFORM1_H
#include "Platform.h"

class Platform1 : public Platform
{
public:
	Platform1(void);
	~Platform1(void);
	void Initialize(int id, XMFLOAT3 pos);
	void Update(float dt);
	void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap);
	void HandleScript();
};
#endif