#include "Direct3D.h"


Direct3D::Direct3D(void)
{
	md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;

	md3dDevice = 0;
	md3dImmediateContext = 0;
	mSwapChain = 0;
	mDepthStencilBuffer = 0;
	mRenderTargetView = 0;
	mDepthStencilView = 0;

	mEnable4xMsaa = false;
	m4xMSAAQuality = 0;

	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));
}


Direct3D::~Direct3D(void)
{
}

bool Direct3D::Init(HWND* mainWindow, int& _clientWidth, int& _clientHeight)
{
	clientWidth = &_clientWidth;
	clientHeight = &_clientHeight;

	// Create D3D device and D3D device context
	// These interfaces are used to interact with the hardware

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;

	HRESULT hr = D3D11CreateDevice(
		0,						// Display adapter (Default)
		md3dDriverType,			// Driver type (For 3D HW Acceleration)
		0,						// Software driver (No software device)
		0/*createDeviceFlags*/,		// Flags (i.e. Debug, Single thread)
		0,						// Feature level (Check what version of D3D is supported. D3D11 forced in this case)
		0,						// Number of feature levels
		D3D11_SDK_VERSION,		// SDK version
		&md3dDevice,			// Returns created device
		&featureLevel,			// Returns feature level
		&md3dImmediateContext); // Return created device context

	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	// D3D11 is forced so check if device actually supports it
	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature level 11 unsupported.", 0, 0);
		return false;
	}

	// Check if 4x MSAA quality is supported
	HR(md3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMSAAQuality));
	assert(m4xMSAAQuality > 0);

	// Instance used for describing the swap chain
	DXGI_SWAP_CHAIN_DESC sd;

	// ----- Start describing swap chain -----
	// Back buffer properties
	sd.BufferDesc.Width = *clientWidth;
	sd.BufferDesc.Height = *clientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Number of multisamples and quality levels,
	// fixed to 4X MSAA or no MSAA in this case
	if (mEnable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m4xMSAAQuality-1;
	}
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Use back buffer as render target
	sd.BufferCount = 1;								  // Number of back buffers to use in swap chain
	sd.OutputWindow = *mainWindow;					  // Specify window we render into
	sd.Windowed = true;								  // Windowed mode or full-screen mode
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;	      // Let display driver select most efficient presentation method
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;									  // Optional flags

	// We have to find and use the same IDXGIFactory that was used to create the device before
	IDXGIDevice* dxgiDevice = 0;
	HR(md3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter),(void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	// Now create the swap chain with the IDXGIFactory we found
	HR(dxgiFactory->CreateSwapChain(md3dDevice, &sd, &mSwapChain));

	// Release temporary COM objects
	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	// Remaining steps are also used when we resize window, so we call that function
	// 1: Create render target view to the swap chain's back buffer
	// 2: Create depth/stencil buffer and view
	// 3: Bind render target view and depth/stencil view to pipeline
	// 4: Set viewport transform
	OnResize();

	return true;
}

void Direct3D::OnResize()
{
	assert(md3dImmediateContext);
	assert(md3dDevice);
	assert(mSwapChain);

	// Release old views because they hold references to buffers we will be destroying
	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);

	// Release depth/stencil buffer
	ReleaseCOM(mDepthStencilBuffer);

	// Resize the swap chain
	HR(mSwapChain->ResizeBuffers(1, *clientWidth, *clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

	// Recreate render target view
	ID3D11Texture2D* backBuffer;
	HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(md3dDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView));
	ReleaseCOM(backBuffer);

	// Create the depth/stencil buffer and view
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = *clientWidth;						// Texture width in texels
	depthStencilDesc.Height = *clientHeight;					// Texture height in texels
	depthStencilDesc.MipLevels = 1;								// Number of mipmap levels
	depthStencilDesc.ArraySize = 1;								// Number of textures in texture array
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// Texel format

	// Set number of multisamples and quality level for the depth/stencil buffer
	// This has to match swap chain MSAA values
	if (mEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMSAAQuality-1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;			// How the texture will be used
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// Where the resource will be bound to the pipeline
	depthStencilDesc.CPUAccessFlags = 0;					// Specify CPU access (Only GPU writes/reads to the depth/buffer)
	depthStencilDesc.MiscFlags = 0;							// Optional flags

	HR(md3dDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer));
	HR(md3dDevice->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView));

	// Bind render target and depth/stencil view to the pipeline
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	// Set the viewport transform
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(*clientWidth);
	mScreenViewport.Height = static_cast<float>(*clientHeight);
	mScreenViewport.MinDepth = 0.0f; // D3D uses a depth buffer range from 0
	mScreenViewport.MaxDepth = 1.0f; // ... to 1

	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);
}

void Direct3D::Shutdown()
{
	// Switch to windowed mode before releasing swap chain
	mSwapChain->SetFullscreenState(FALSE, NULL);

	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mSwapChain);
	ReleaseCOM(mDepthStencilBuffer);

	if (md3dImmediateContext)
		md3dImmediateContext->ClearState();

	ReleaseCOM(md3dImmediateContext);
	ReleaseCOM(md3dDevice);
}

ID3D11Device* Direct3D::GetDevice() const
{
	return md3dDevice;
}

ID3D11DeviceContext* Direct3D::GetImmediateContext() const
{
	return md3dImmediateContext;
}

ID3D11RenderTargetView* Direct3D::GetRenderTargetView() const
{
	return mRenderTargetView;
}

ID3D11DepthStencilView* Direct3D::GetDepthStencilView() const
{
	return mDepthStencilView;
}

D3D11_VIEWPORT Direct3D::GetScreenViewport() const
{
	return mScreenViewport;
}

IDXGISwapChain* Direct3D::GetSwapChain() const
{
	return mSwapChain;
}
