#ifndef DUCK_H
#define DUCK_H
#include "Platform.h"

class Duck : public Platform
{
public:
	Duck(void);
	~Duck(void);
	void Initialize(XMFLOAT3 pos);
	void Update(float dt);
	void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap);
	void HandleScript();
};
#endif