#include "AnimatedEntity.h"

AnimatedEntity::AnimatedEntity( void )
{

}

AnimatedEntity::AnimatedEntity(GenericSkinnedModel* model, XMFLOAT3 position)
{
	mInstance.model = model;
	mInstance.isVisible = true;
	mInstance.TimePos = 0.0f;
	mInstance.ClipName = "combinedAnim_0";
	mInstance.FinalTransforms.resize(mInstance.model->skinnedData.Bones.size());
	this->Position = position;
	this->Scale = XMFLOAT3(1, 1, 1);
	this->Rotation = 0.0f;

	SetPosition(position);
}


AnimatedEntity::~AnimatedEntity(void)
{
}

void AnimatedEntity::Draw( ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap )
{
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dc->IASetInputLayout(InputLayouts::PosNormalTexTanSkinned);

	UINT stride = sizeof(Vertex::PosNormalTexTanSkinned);
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
	Effects::NormalMapFX->SetBoneTransforms(&mInstance.FinalTransforms[0], mInstance.FinalTransforms.size());

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
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

void AnimatedEntity::SetPosition(XMFLOAT3 pos)
{
	this->Position = pos;

	XMMATRIX modelScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX modelRot = XMMatrixRotationY(0.0f);
	XMMATRIX modelOffset = XMMatrixTranslation(Position.x, Position.y, Position.z);
	XMStoreFloat4x4(&mInstance.world, modelScale*modelRot*modelOffset);
}

void AnimatedEntity::RotateXYZ(XMFLOAT3 rot, float yaw, XMVECTOR Up)
{
	XMMATRIX modelRot = XMMatrixRotationX(rot.x) * XMMatrixRotationY(rot.y) * XMMatrixRotationZ(rot.z) * XMMatrixRotationAxis(Up, yaw);
	XMMATRIX modelScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX modelOffset = XMMatrixTranslation(Position.x, Position.y, Position.z);
	XMStoreFloat4x4(&mInstance.world, modelScale*modelRot*modelOffset);
}
