#ifndef PLATFORM2_H
#define PLATFORM2_H
#include "Platform.h"

class Platform2 : public Platform
{
public:
	Platform2(void);
	~Platform2(void);
	void Initialize(int id, XMFLOAT3 pos);
	void Update(float dt);
	void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* at, Camera* camera, ShadowMap* shadowMap);
	void HandleScript();
};
#endif