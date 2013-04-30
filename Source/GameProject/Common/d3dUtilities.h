#ifndef D3DUTILITIES_H_
#define D3DUTILITIES_H_

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <d3dx11.h>
#include "d3dx11effect.h"
#include <xnamath.h>
#include <dxerr.h>
#include <cassert>
#include <ctime>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "LightDef.h"
#include "MathHelper.h"

//===============================================================
// Simple d3d error checker
//===============================================================
#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
	#define HR(x)												\
	{															\
	HRESULT hr = (x);											\
	if(FAILED(hr))												\
		{														\
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true);		\
		}														\
	}
	#endif

#else
	#ifndef HR
	#define HR(x) (x)
	#endif
#endif 

//===============================================================
// Macros
//===============================================================
// Macro for releasing COM objects
#define ReleaseCOM(x) { if (x) { x->Release(); x = 0; } }

// Macro for safe deletion
#define SafeDelete(x) { delete x; x = 0; }

//==============================================================
// D3D functions
//==============================================================
class d3dHelper
{
public:
	// Creates a 2D array with textures
	// Does not work with compressed formats
	static ID3D11ShaderResourceView* CreateTexture2DArraySRV(
		ID3D11Device* device, ID3D11DeviceContext* context,
		std::vector<std::wstring>& filenames,
		DXGI_FORMAT format = DXGI_FORMAT_FROM_FILE,
		UINT filter = D3DX11_FILTER_NONE, 
		UINT mipFilter = D3DX11_FILTER_LINEAR);

	static ID3D11ShaderResourceView* CreateRandomTexture1DSRV(ID3D11Device* device);
};

//==============================================================
// Colors
//==============================================================
#define XMGLOBALCONST extern CONST __declspec(selectany)
namespace Colors
{
	XMGLOBALCONST XMVECTORF32 White     = {1.0f, 1.0f, 1.0f, 1.0f};
	XMGLOBALCONST XMVECTORF32 Black     = {0.0f, 0.0f, 0.0f, 1.0f};
	XMGLOBALCONST XMVECTORF32 Red       = {1.0f, 0.0f, 0.0f, 1.0f};
	XMGLOBALCONST XMVECTORF32 Green     = {0.0f, 1.0f, 0.0f, 1.0f};
	XMGLOBALCONST XMVECTORF32 Blue      = {0.0f, 0.0f, 1.0f, 1.0f};
	XMGLOBALCONST XMVECTORF32 Yellow    = {1.0f, 1.0f, 0.0f, 1.0f};
	XMGLOBALCONST XMVECTORF32 Cyan      = {0.0f, 1.0f, 1.0f, 1.0f};
	XMGLOBALCONST XMVECTORF32 Magenta   = {1.0f, 0.0f, 1.0f, 1.0f};

	XMGLOBALCONST XMVECTORF32 Silver    = {0.75f, 0.75f, 0.75f, 1.0f};
	XMGLOBALCONST XMVECTORF32 LightSteelBlue = {0.69f, 0.77f, 0.87f, 1.0f};
}

#endif