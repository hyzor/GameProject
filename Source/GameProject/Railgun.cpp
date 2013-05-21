#include "Railgun.h"

Railgun::Railgun()
	: Weapon()
{
}

Railgun::~Railgun()
{
	delete mParticleSys;
}

void Railgun::Update(float dt, float gameTime)
{
	Weapon::Update(dt, gameTime);

	mParticleSys->update(dt, gameTime);
}

void Railgun::Init(Properties properties, ID3D11Device* device, ID3D11DeviceContext* dc, std::string modelName, XMFLOAT3 pos)
{
	Weapon::Init(properties, modelName, pos);
	//mParticleSys = particleSystem;

	mParticleSys = new ParticleSystem();

	mRandomTexSRV = d3dHelper::CreateRandomTexture1DSRV(device);

	std::vector<std::wstring> flares;
	//flares.push_back(L"Data\\Textures\\flare0.dds");
	flares.push_back(L"Data\\Textures\\particle.dds");
	mLaserTexSRV = d3dHelper::CreateTexture2DArraySRV(device, dc, flares);

	mParticleSys->init(device, Effects::LaserFX, mLaserTexSRV, mRandomTexSRV, 1000);
	//mParticleSys->setEmitPos(XMFLOAT3(1.0f, 300.0f, 50.0f));
}

void Railgun::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, Camera* camera, ShadowMap* shadowMap)
{
	// Draws the weapon model
	Weapon::Draw(dc, tech, camera, shadowMap);

	// Draw particle system
	mParticleSys->setEyePos(camera->GetPosition());

	// Dont draw particle system after specified time
	if (mParticleSys->getAge() < 1.0f)
		mParticleSys->draw(dc, camera);

	// Restore blend state
	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	dc->OMSetBlendState(0, blendFactor, 0xffffffff);
}

bool Railgun::FireProjectile(XMFLOAT3 pos, XMFLOAT3 dir)
{
	if (Weapon::FireProjectile(pos, dir))
	{
		// Shoot ray and emit particle system
		mParticleSys->setEmitPos(XMFLOAT3(pos.x, pos.y, pos.z));
		mParticleSys->setEmitDir(dir);
		mParticleSys->reset();

		return true;
	}

	return false;
}