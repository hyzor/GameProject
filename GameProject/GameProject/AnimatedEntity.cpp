#include "AnimatedEntity.h"

AnimatedEntity::AnimatedEntity( void )
{

}

AnimatedEntity::AnimatedEntity(GenericSkinnedModel* model, XMFLOAT3 position)
{
	mInstance.model = model;
	mInstance.isVisible = true;
	this->Position = position;
	this->Scale = XMFLOAT3(1, 1, 1);
	this->Rotation = 0.0f;

	XMMATRIX modelScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX modelRot = XMMatrixRotationY(0.0f);
	XMMATRIX modelOffset = XMMatrixTranslation(Position.x, Position.y, Position.z);
	XMStoreFloat4x4(&mInstance.world, modelScale*modelRot*modelOffset);
}


AnimatedEntity::~AnimatedEntity(void)
{
}

void AnimatedEntity::Draw( ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap )
{
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dc->IASetInputLayout(InputLayouts::Basic32);

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	XMMATRIX view = mCamera->GetViewMatrix();
	XMMATRIX proj = mCamera->GetProjMatrix();
	XMMATRIX viewproj = mCamera->GetViewProjMatrix();

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);

	world = XMLoadFloat4x4(&mInstance.world);
	worldInvTranspose = MathHelper::InverseTranspose(world);
	worldViewProj = world*view*proj;

	Effects::NormalMapFX->SetWorld(world);
	Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
	Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
	Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
	Effects::NormalMapFX->SetShadowTransform(world*XMLoadFloat4x4(&shadowMap->GetShadowTransform()));
	Effects::NormalMapFX->SetShadowMap(shadowMap->getDepthMapSRV());
	Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));
	//Effects::NormalMapFX->SetBoneTransforms(mInstance.model->skinnedData.Transforms, )

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		Effects::BasicTessFX->SetFogColor(Colors::Silver);

		for (UINT i = 0; i < mInstance.model->numMeshes; ++i)
		{
			UINT matIndex = mInstance.model->meshes[i].mMaterialIndex;

			Effects::NormalMapFX->SetMaterial(mInstance.model->mat[matIndex]);

			Effects::NormalMapFX->SetDiffuseMap(mInstance.model->diffuseMapSRV[matIndex]);

			activeTech->GetPassByIndex(p)->Apply(0, dc);
			mInstance.model->meshes[i].draw(dc);
		}
	}
}

void AnimatedEntity::Update(float dt)
{
	mInstance.Update(dt);
}
