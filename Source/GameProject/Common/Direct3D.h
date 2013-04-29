#ifndef DIRECT3D_H_
#define DIRECT3D_H_

#include "d3dUtilities.h"

class Direct3D
{
public:
	Direct3D(void);
	~Direct3D(void);

	bool Init(HWND* mainWindow, int& _clientWidth, int& _clientHeight);
	void OnResize();
	void Shutdown();

	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetImmediateContext() const;
	ID3D11RenderTargetView* GetRenderTargetView() const;
	ID3D11DepthStencilView* GetDepthStencilView() const;
	D3D11_VIEWPORT GetScreenViewport() const;
	IDXGISwapChain* GetSwapChain() const;

private:
	ID3D11Device* md3dDevice;
	ID3D11DeviceContext* md3dImmediateContext;
	IDXGISwapChain* mSwapChain;
	ID3D11Texture2D* mDepthStencilBuffer;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11DepthStencilView* mDepthStencilView;
	D3D11_VIEWPORT mScreenViewport;

	D3D_DRIVER_TYPE md3dDriverType;

	bool mEnable4xMsaa;
	UINT m4xMSAAQuality;

	int* clientWidth;
	int* clientHeight;
};

#endif