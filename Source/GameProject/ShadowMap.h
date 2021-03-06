//------------------------------------------------------------------------------------
// File: ShadowMap.h
//
// This class encapsulates a depth/stencil buffer, views and viewport for
// shadow mapping
//------------------------------------------------------------------------------------

#ifndef SHADOWMAP_H_
#define SHADOWMAP_H_

#include <d3dUtilities.h>
#include "xnacollision.h"
#include "GenericModel.h"
#include <Camera.h>
#include <Direct3D.h>

class ShadowMap
{
public:
	ShadowMap(ID3D11Device* device, UINT width, UINT height);
	~ShadowMap(void);

	ID3D11ShaderResourceView* getDepthMapSRV();

	void BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc);
	void SetResolution(ID3D11Device* device, UINT width, UINT height);

	UINT GetWidth() const;
	UINT GetHeight() const;
	XMFLOAT4X4 GetShadowTransform() const;

	void BuildShadowTransform(const DirectionalLight& light,
							const XNA::Sphere& sceneBounds);

	void DrawSceneToShadowMap(
					const std::vector<GenericModelInstance>& modelInstances,
					const Camera& camera,
					ID3D11DeviceContext* deviceContext);

private:
	UINT mWidth;
	UINT mHeight;

	XMFLOAT4X4 mLightView;
	XMFLOAT4X4 mLightProj;
	XMFLOAT4X4 mShadowTransform;

	ID3D11ShaderResourceView* mDepthMapSRV;
	ID3D11DepthStencilView* mDepthMapDSV;

	D3D11_VIEWPORT mViewport;

	void CreateShadowMap(ID3D11Device* device, UINT width, UINT height);
};

#endif