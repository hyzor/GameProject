#ifndef D3D11APP_H
#define D3D11APP_H

#include "d3dUtilities.h"
#include "GameTimer.h"
#include <string>

// Direct Input
// #pragma comment (lib, "dinput8.lib")
// #pragma comment (lib, "dxguid.lib")
// #include <dinput.h>

#include "DirectInput.h"
#include "Direct3D.h"

class D3D11App
{
public:
	D3D11App(HINSTANCE hInstance);
	~D3D11App(void);

	HINSTANCE AppInst() const;
	HWND MainWnd() const;
	float AspectRatio() const;

	UINT GetWindowWidth() const;
	UINT GetWindowHeight() const;

	// Game loop
	int Run();

	virtual bool Init();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	virtual void OnResize(); 
	virtual void UpdateScene(float dt)=0;
	virtual void DrawScene()=0;

	// Convenience overrides for handling mouse input
	virtual void OnMouseDown(WPARAM btnState, int x, int y){ }
	virtual void OnMouseUp(WPARAM btnState, int x, int y)  { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y){ }

//	virtual void DetectInput(float dt) = 0;

protected:
	bool InitMainWindow();
	//bool InitDirect3D();

	// Direct Input
	//bool InitDirectInput();

	void CalculateFrameStats();

	HINSTANCE mhAppInst;
	HWND mhMainWnd;
	bool mAppPaused;
	bool mMinimized;
	bool mMaximized;
	bool mResizing;
//	UINT m4xMSAAQuality;

// 	ID3D11Device* md3dDevice;
// 	ID3D11DeviceContext* md3dImmediateContext;
// 	IDXGISwapChain* mSwapChain;
// 	ID3D11Texture2D* mDepthStencilBuffer;
// 	ID3D11RenderTargetView* mRenderTargetView;
// 	ID3D11DepthStencilView* mDepthStencilView;
// 	D3D11_VIEWPORT mScreenViewport;

	// Customized values
	int mClientWidth;
	int mClientHeight;
	std::wstring mMainWndCaption;
//	D3D_DRIVER_TYPE md3dDriverType;
//	bool mEnable4xMsaa;

	GameTimer mTimer;

	//---------------------------------------
	// Direct Input
	//---------------------------------------
// 	// COM Interfaces
// 	IDirectInputDevice8* DiKeyboard;
// 	IDirectInputDevice8* DiMouse;
// 
// 	// Mouse state
// 	DIMOUSESTATE mouseLastState;
// 
// 	// Keyboard input
// 	LPDIRECTINPUT8 DirectInput;

	DirectInput* mDirectInput;

	Direct3D* mDirect3D;

};

#endif

