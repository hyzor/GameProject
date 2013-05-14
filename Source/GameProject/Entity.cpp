#include "Entity.h"


Entity::Entity(void)
{
}

Entity::Entity(GenericModel* Model, XMFLOAT3 Position)
{
	mInstance.model = Model;
	mInstance.isVisible = true;
	this->Position = Position;
	this->Scale = XMFLOAT3(1, 1, 1);
	this->Rotation = 0.0f;

	XMMATRIX modelScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX modelRot = XMMatrixRotationY(0.0f);
	XMMATRIX modelOffset = XMMatrixTranslation(Position.x, Position.y, Position.z);
	XMStoreFloat4x4(&mInstance.world, modelScale*modelRot*modelOffset);
}


Entity::~Entity(void)
{
}

void Entity::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera, ShadowMap* shadowMap)
{
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dc->IASetInputLayout(InputLayouts::PosNormalTexTan);

	UINT stride = sizeof(Vertex::PosNormalTexTan);
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

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		//Effects::BasicTessFX->SetFogColor(Colors::Silver);

		for (UINT i = 0; i < mInstance.model->meshCount; ++i)
		{
			UINT matIndex = mInstance.model->meshes[i].MaterialIndex;

			Effects::NormalMapFX->SetMaterial(mInstance.model->mat[matIndex]);

			Effects::NormalMapFX->SetDiffuseMap(mInstance.model->diffuseMapSRV[matIndex]);

			Effects::NormalMapFX->setNormalMap(mInstance.model->normalMapSRV[matIndex]);

			activeTech->GetPassByIndex(p)->Apply(0, dc);
			mInstance.model->meshes[i].draw(dc);
		}
	}
}

void Entity::RotateEntityX(float rot)
{
	this->Rotation = rot;
	XMMATRIX modelRot = XMMatrixRotationX(rot);
	XMMATRIX modelScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX modelOffset = XMMatrixTranslation(Position.x, Position.y, Position.z);
	XMStoreFloat4x4(&mInstance.world, modelScale*modelRot*modelOffset);
}

void Entity::RotateEntityY(float rot)
{
	this->Rotation = rot;
	XMMATRIX modelRot = XMMatrixRotationY(rot);
	XMMATRIX modelScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX modelOffset = XMMatrixTranslation(Position.x, Position.y, Position.z);
	XMStoreFloat4x4(&mInstance.world, modelScale*modelRot*modelOffset);
}
void Entity::RotateEntityZ(float rot)
{
	this->Rotation = rot;
	XMMATRIX modelRot = XMMatrixRotationZ(rot);
	XMMATRIX modelScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX modelOffset = XMMatrixTranslation(Position.x, Position.y, Position.z);
	XMStoreFloat4x4(&mInstance.world, modelScale*modelRot*modelOffset);
}
