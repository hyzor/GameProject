#ifndef PLATFORM4_H
#define PLATFORM4_H
#include "Platform.h"

class Platform4 : public Platform
{
public:
	Platform4(void);
	~Platform4(void);
	void Initialize(int id, XMFLOAT3 pos);
	void Update(float dt);
	void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap);
	void HandleScript();
};
#endif