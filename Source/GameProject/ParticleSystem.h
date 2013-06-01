#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <windows.h>
#include <xnamath.h>
#include <d3d11.h>
#include "LightDef.h"
#include <vector>
#include "D3D11App.h"
#include <fstream>
#include "Vertex.h"
#include "d3dx11effect.h"
#include "Effects.h"
#include <algorithm>
#include "Camera.h"

class ParticleSystem
{
public:
	ParticleSystem(void);
	~ParticleSystem(void);

	// Time since system was reset
	float getAge() const;

	void setEyePos(const XMFLOAT3& eyePosW);
	void setEmitPos(const XMFLOAT3& emitPosW);
	void setEmitDir(const XMFLOAT3& emitDirW);

	void SetHitPos(const XMFLOAT3& hitPosW) { mHitPosW = hitPosW; }

	void init(ID3D11Device* device, ParticleEffect* fx,
		ID3D11ShaderResourceView* texArraySRV, ID3D11ShaderResourceView* randomTexSRV, UINT maxParticles);

	void reset();
	void update(float dt, float gameTime);
	void draw(ID3D11DeviceContext* dc, Camera* cam);
private:
	void BuildVB(ID3D11Device* device);

	UINT mMaxParticles;
	bool mFirstRun;

	float mGameTime;
	float mTimeStep;
	float mAge;

	XMFLOAT3 mEyePosW;
	XMFLOAT3 mEmitPosW;
	XMFLOAT3 mEmitDirW;

	XMFLOAT3 mHitPosW;

	ParticleEffect* mFX;

	ID3D11Buffer* mInitVB;	
	ID3D11Buffer* mDrawVB;
	ID3D11Buffer* mStreamOutVB;

	ID3D11ShaderResourceView* mTexArraySRV;
	ID3D11ShaderResourceView* mRandomTexSRV;
};

#endif