#ifndef GUI_H_
#define GUI_H_
#define WIN32_LEAN_AND_MEAN
#include <FW1FontWrapper.h>
#include <d3dUtilities.h>
#include <D3DX10math.h>
#include "Common\Camera.h"
#include "Game.h"

using namespace std;

class GUI
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	GUI();
	~GUI();
	void Init(ID3D11Device *device);
	void Render(ID3D11DeviceContext *context);
	bool InMenu() const;
	bool Update(DirectInput* di);
	void Release();

private:
	void drawText(ID3D11DeviceContext *context, WCHAR* text, XMFLOAT2 pos, float fontSize, int color) const;
	void DrawMenu(ID3D11DeviceContext *context);
	void drawCrossHair(ID3D11DeviceContext *context);

private:
	IFW1Factory* mFW1Factory;
	IFW1FontWrapper* mFontWrapper;

	int menuIndex;
	bool menuActive;
	wchar_t **menuItems;
	float fontSize;

	bool escapeDown;
	bool upArrowDown;
	bool downArrowDown;
	bool enterDown;
};

#endif
