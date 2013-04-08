#include "D3D11App.h"
#include <WindowsX.h>

namespace
{
	// Only used for forwarding Windows messages from a global window procedure
	// to our member specific window procedure
	D3D11App* gd3d11App = 0;
}

D3D11App::D3D11App(HINSTANCE hInstance)
	: mhAppInst(hInstance),
	mMainWndCaption(L"Direct3D Application"),
	mClientWidth(1024),
	mClientHeight(768),
	mhMainWnd(0),
	mAppPaused(false),
	mMinimized(false),
	mMaximized(false),
	mResizing(false)
{
	// Bind pointer to this object
	gd3d11App = this;
}

D3D11App::~D3D11App(void)
{
	if (mDirectInput)
	{
		mDirectInput->Shutdown();
		SafeDelete(mDirectInput);
	}

	if (mDirect3D)
	{
		mDirect3D->Shutdown();
		SafeDelete(mDirect3D);
	}
}

//================================================================
// Initialize application
//================================================================
bool D3D11App::Init()
{
	// Have to create Direct3D object before initializing
	// window, basically to initialize d3dDevice to 0
	mDirect3D = new Direct3D();

	if (!mDirect3D)
		return false;

	//-------------------------------------------
	// Init Win32 window
	//-------------------------------------------
	if (!InitMainWindow())
		return false;

	//-------------------------------------------
	// Init Direct3D
	//-------------------------------------------

	if (!mDirect3D->Init(&mhMainWnd, mClientWidth, mClientHeight))
		return false;

	//-------------------------------------------
	// Init Direct Input
	//-------------------------------------------
	mDirectInput = new DirectInput();

	if (!mDirectInput)
		return false;

	if (!mDirectInput->Init(&mhAppInst, &mhMainWnd, mClientWidth, mClientHeight))
		return false;

	return true;
}

//================================================================
// Application loop
//================================================================
int D3D11App::Run()
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));

	mTimer.reset();

	while(msg.message != WM_QUIT)
	{
		// If there are Window messages, process them
		if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		// Else run the game loop
		else
		{
			mTimer.tick();

			if (!mAppPaused)
			{
				CalculateFrameStats();
				//DetectInput(mTimer.getDeltaTime());
				mDirectInput->Update();
				UpdateScene(mTimer.getDeltaTime());
				DrawScene();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return (int)msg.wParam;
}

//=========================================================================
// Win32 messages
//=========================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward msg to this object's MsgProc. In case where we get a msg
	// before "CreateWindow" has finished. (That is, before mhMainWnd is valid)
	return gd3d11App->MsgProc(hWnd, msg, wParam, lParam);
}

LRESULT D3D11App::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	// Window being destroyed
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;

	// Window activated or deactivated (in focus or not)
	case WM_ACTIVATE:
		if(LOWORD(wParam) == WA_INACTIVE)
		{
			mAppPaused = true;
			mTimer.stop();
		}
		else
		{
			mAppPaused = false;
			mTimer.start();
		}
		return 0;

	// Window is resized
	case WM_SIZE:
		// Get the new client area dimensions
		mClientWidth = LOWORD(lParam);
		mClientHeight = HIWORD(lParam);

		if (mDirect3D->GetDevice())
		{
			if (wParam == SIZE_MINIMIZED)
			{
				mAppPaused = true;
				mMinimized = true;
				mMaximized = false;
			}

			else if(wParam == SIZE_MAXIMIZED)
			{
				mAppPaused = false;
				mMinimized = false;
				mMaximized = true;
				OnResize();
			}

			else if(wParam == SIZE_RESTORED)
			{

				// If we are restoring from minimized state
				if (mMinimized)
				{
					mAppPaused = false;
					mMinimized = false;
					OnResize();
				}

				// If we are restoring from maximized state
				else if (mMaximized)
				{
					mAppPaused = false;
					mMaximized = false;
					OnResize();
				}
				else if(mResizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

	// If the user grabs the resize bars
	case WM_ENTERSIZEMOVE:
		mAppPaused = true;
		mResizing  = true;
		mTimer.stop();
		return 0;

	// If the user releases the resize bars
	// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing  = false;
		mTimer.start();
		OnResize();
		return 0;

	// The WM_MENUCHAR message is sent when a menu is active and the user presses 
	// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

	// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200; 
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

//====================================================================
// Initialize Win32 window
//====================================================================
bool D3D11App::InitMainWindow()
{
	// Register the Window class
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhAppInst;
	wc.hIcon = ::LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = ::LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"Window";

	if (!RegisterClass(&wc))
	{
		::MessageBox(0, L"Failed - Register WNDCLASS", 0, MB_OK);
		return false;
	}

	// Set Window dimensions
	RECT R = { 0, 0, mClientWidth, mClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	// Create Window
	mhMainWnd = ::CreateWindow(L"Window", mMainWndCaption.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		width, height, 0, 0, mhAppInst, 0);

	if (mhMainWnd == 0)
	{
		::MessageBox(0, L"Failed - Creation WNDCLASS", 0, MB_OK);
		return false;
	}

	::ShowWindow(mhMainWnd, SW_SHOW);
	::UpdateWindow(mhMainWnd);

	return true;
}

// Calculate aspect ratio
float D3D11App::AspectRatio() const
{
	return static_cast<float>(mClientWidth) / mClientHeight;
}

void D3D11App::CalculateFrameStats()
{
	// Computes average FPS, and average time it takes to render one frame
	static int frameCount = 0;
	static float timeElapsed = 0.0f;

	frameCount++;

	if ((mTimer.getTimeElapsedS() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCount;
		float msPerFrame = 1000.0f / fps;

		std::wostringstream outsstream;
		outsstream.precision(6);
		outsstream	<< mMainWndCaption << L"   "
					<< L"FPS: " << fps << L"   "
					<< L"Frame Time: " << msPerFrame << L" (ms)";
		SetWindowText(mhMainWnd, outsstream.str().c_str());

		// Reset for next average
		frameCount = 0;
		timeElapsed += 1.0f;
	}
}

UINT D3D11App::GetWindowWidth() const
{
	return mClientWidth;
}

UINT D3D11App::GetWindowHeight() const
{
	return mClientHeight;
}

void D3D11App::OnResize()
{
	mDirect3D->OnResize();
}