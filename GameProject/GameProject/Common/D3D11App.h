#ifndef D3D11APP_H
#define D3D11APP_H

#include "d3dUtilities.h"
#include "GameTimer.h"
#include <string>

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

	void SetResolution(UINT Width, UINT Height);
	void SetFullscreen(bool toFullscreen);
	void SwitchFullscreen();

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

protected:
	bool InitMainWindow();

	void CalculateFrameStats();

	BOOL mIsFullscreen;

	HINSTANCE mhAppInst;
	HWND mhMainWnd;
	bool mAppPaused;
	bool mMinimized;
	bool mMaximized;
	bool mResizing;

	// Customized values
	int mClientWidth;
	int mClientHeight;
	std::wstring mMainWndCaption;

	GameTimer mTimer;

	DirectInput* mDirectInput;

	Direct3D* mDirect3D;

};

#endif

