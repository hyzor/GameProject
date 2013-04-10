//------------------------------------------------------------------------------------
// File: ShadowMap.cpp
//
// This class encapsulates a depth/stencil buffer, views and viewport for
// shadow mapping
//------------------------------------------------------------------------------------
#include "ShadowMap.h"


ShadowMap::ShadowMap(ID3D11Device* device, UINT width, UINT height)
	: mDepthMapDSV(0), mDepthMapSRV(0)
{
	createShadowMap(device, width, height);
}


ShadowMap::~ShadowMap(void)
{
	ReleaseCOM(mDepthMapSRV);
	ReleaseCOM(mDepthMapDSV);
}

ID3D11ShaderResourceView* ShadowMap::getDepthMapSRV()
{
	return mDepthMapSRV;
}

void ShadowMap::BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc)
{
	dc->RSSetViewports(1, &mViewport);

	// We set null render target because we only draw to depth buffer.
	// This will disable color writes
	ID3D11RenderTargetView* renderTargets[1] = {0};
	dc->OMSetRenderTargets(1, renderTargets, mDepthMapDSV);

	dc->ClearDepthStencilView(mDepthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void ShadowMap::setResolution(ID3D11Device* device, UINT width, UINT height)
{	
	// Release old SRV and DSV
	ReleaseCOM(mDepthMapSRV);
	ReleaseCOM(mDepthMapDSV);

	createShadowMap(device, width, height);
}

void ShadowMap::createShadowMap(ID3D11Device* device, UINT width, UINT height)
{
	// Set new dimensions
	mWidth = width;
	mHeight = height;

	// Specify viewport to match shadow map dimensions
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = static_cast<float>(mWidth);
	mViewport.Height = static_cast<float>(mHeight);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	// Texture description
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	// Texture format is typeless because the DSV and SRV use different formats
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	// Now create the texture
	ID3D11Texture2D* depthMap = 0;
	HR(device->CreateTexture2D(&texDesc, 0, &depthMap));

	// DSV description
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	// Create DSV
	HR(device->CreateDepthStencilView(depthMap, &dsvDesc, &mDepthMapDSV));

	// SRV description
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	// Create SRV
	HR(device->CreateShaderResourceView(depthMap, &srvDesc, &mDepthMapSRV));

	// Now release depth map reference because view already saved a reference
	ReleaseCOM(depthMap);
}

UINT ShadowMap::getWidth() const
{
	return mWidth;
}

UINT ShadowMap::getHeight() const
{
	return mHeight;
}

void ShadowMap::buildShadowTransform(const DirectionalLight& light, const XNA::Sphere& sceneBounds)
{
	// Only first "main" light casts a shadow
	// So get light direction and position from first light
	XMVECTOR lightDir = XMLoadFloat3(&light.Direction);

	XMVECTOR lightPos = -2.0f*sceneBounds.Radius*lightDir;

	XMVECTOR targetPos = XMLoadFloat3(&sceneBounds.Center);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

	// Transform bounding sphere to light space
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));

	// Orthogonal frustum in light space encloses scene
	float l = sphereCenterLS.x - sceneBounds.Radius;
	float b = sphereCenterLS.y - sceneBounds.Radius;
	float n = sphereCenterLS.z - sceneBounds.Radius;
	float r = sphereCenterLS.x + sceneBounds.Radius;
	float t = sphereCenterLS.y + sceneBounds.Radius;
	float f = sphereCenterLS.z + sceneBounds.Radius;
	XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX S = V*P*T;

	XMStoreFloat4x4(&mLightView, V);
	XMStoreFloat4x4(&mLightProj, P);
	XMStoreFloat4x4(&mShadowTransform, S);
}

void ShadowMap::drawSceneToShadowMap( 
	const std::vector<GenericModelInstance>& modelInstances, 
	const Camera& camera,
	ID3D11DeviceContext* deviceContext)
{
	XMMATRIX view = XMLoadFloat4x4(&mLightView);
	XMMATRIX proj = XMLoadFloat4x4(&mLightProj);
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	Effects::BuildShadowMapFX->SetEyePosW(camera.getPosition());
	Effects::BuildShadowMapFX->SetViewProj(viewProj);

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	ID3DX11EffectTechnique* tech = Effects::BuildShadowMapFX->TessBuildShadowMapTech;

	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);

	//------------------------------------------------------------------
	// Draw opaque tessellated objects
	//------------------------------------------------------------------
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	deviceContext->IASetInputLayout(InputLayouts::PosNormalTexTan);

	for (UINT pass = 0; pass < techDesc.Passes; ++pass)
	{
		for (UINT i = 0; i < modelInstances.size(); ++i)
		{
			world = XMLoadFloat4x4(&modelInstances[i].world);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BuildShadowMapFX->SetWorld(world);
			Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
			Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

			tech->GetPassByIndex(pass)->Apply(0, deviceContext);

			for (UINT j = 0; j < modelInstances[i].model->meshCount; ++j)
			{
				modelInstances[i].model->meshes[i].draw(deviceContext);
			}
		}
	}

	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.
	deviceContext->HSSetShader(0, 0, 0);
	deviceContext->DSSetShader(0, 0, 0);

	deviceContext->RSSetState(0);
}

XMFLOAT4X4 ShadowMap::getShadowTransform() const
{
	return mShadowTransform;
}
