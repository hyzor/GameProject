//------------------------------------------------------------------------------------
// File: Ssao.h
//
// Encapsulates data and methods used for screen space ambient occlusion
//------------------------------------------------------------------------------------

#ifndef SSAO_H_
#define SSAO_H_

#include <d3dUtilities.h>

class Ssao
{
public:
	Ssao(ID3D11Device* device, ID3D11DeviceContext* dc, int width, int height, float fovy, float farZ);
	~Ssao(void);

	ID3D11ShaderResourceView* NormalDepthSRV();
	ID3D11ShaderResourceView* AmbientSRV();

	// Called when the back buffer is resized
	void OnSize(int width, int height, float fovy, float farZ);

	// Changes render target to the NormalDepth render target
	void SetNormalDepthRenderTarget(ID3D11DepthStencilView* dsv);

	// Changes render target to Ambient render target and draws a
	// fullscreen quad
	void ComputeSsao(const Camera& camera);

	// Blur ambient map to smooth out noise (by only taking a few random samples per pixel)
	void BlurAmbientMap(int blurCount);

private:
	void BlurAmbientMap(ID3D11ShaderResourceView* inputSRV, ID3D11RenderTargetView* outputRTV, bool horzBlur);

	void BuildFrustumFarCorners(float fovy, float farZ);

	void BuildFullScreenQuad();

	void BuildTextureViews();
	void ReleaseTextureViews();

	void BuildRandomVectorTexture();

	void BuildOffsetVectors();

	void DrawFullScreenQuad();

	ID3D11Device* md3dDevice;
	ID3D11DeviceContext* mDC;

	ID3D11Buffer* mScreenQuadVB;
	ID3D11Buffer* mScreenQuadIB;

	ID3D11ShaderResourceView* mRandomVectorSRV;

	ID3D11RenderTargetView* mNormalDepthRTV;
	ID3D11ShaderResourceView* mNormalDepthSRV;

	// Need two for swapping during blur.
	ID3D11RenderTargetView* mAmbientRTV0;
	ID3D11ShaderResourceView* mAmbientSRV0;

	ID3D11RenderTargetView* mAmbientRTV1;
	ID3D11ShaderResourceView* mAmbientSRV1;


	UINT mRenderTargetWidth;
	UINT mRenderTargetHeight;

	XMFLOAT4 mFrustumFarCorner[4];

	XMFLOAT4 mOffsets[14];

	D3D11_VIEWPORT mAmbientMapViewport;
};

#endif