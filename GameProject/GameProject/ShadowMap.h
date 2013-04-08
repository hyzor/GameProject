//------------------------------------------------------------------------------------
// File: ShadowMap.h
//
// This class encapsulates a depth/stencil buffer, views and viewport for
// shadow mapping
//------------------------------------------------------------------------------------

#ifndef SHADOWMAP_H_
#define SHADOWMAP_H_

#include <d3dUtilities.h>

class ShadowMap
{
public:
	ShadowMap(ID3D11Device* device, UINT width, UINT height);
	~ShadowMap(void);

	ID3D11ShaderResourceView* getDepthMapSRV();

	void BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc);
	void setResolution(ID3D11Device* device, UINT width, UINT height);

private:
	UINT mWidth;
	UINT mHeight;

	ID3D11ShaderResourceView* mDepthMapSRV;
	ID3D11DepthStencilView* mDepthMapDSV;

	D3D11_VIEWPORT mViewport;

	void createShadowMap(ID3D11Device* device, UINT width, UINT height);
};

#endif