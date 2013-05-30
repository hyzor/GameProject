#ifndef GUI_H_
#define GUI_H_
#define WIN32_LEAN_AND_MEAN
#include <FW1FontWrapper.h>
#include <d3dUtilities.h>
#include <D3DX10math.h>
#include <string>
#include "Common\Camera.h"
#include "Settings.h"
#include "Game.h"

using namespace std;

#define Gui GUI::GetInstance()
#define SCREEN_WIDTH Settings::GetInstance()->GetData().Width
#define SCREEN_HEIGHT Settings::GetInstance()->GetData().Height

class GUI
{
public:
	
	static enum state
	{
		Game,
		Pause
	};

	static GUI* GetInstance();
	void Init(ID3D11Device *device);
	void Render(ID3D11DeviceContext *context, XMFLOAT3 playerPos);
	bool InMenu() const;
	bool Update(DirectInput* di, float dt);
	void drawText(ID3D11DeviceContext *context, WCHAR* text, XMFLOAT2 pos, float fontSize, int color) const;
	void setState(int state);
	void Release();
	void AddEventText(std::wstring text, float timeToLive);

private:
	static GUI* mInstance;
	GUI();
	~GUI();
	void DrawMenu(ID3D11DeviceContext *context);
	void drawCrossHair(ID3D11DeviceContext *context);
	void drawScoreBoard(ID3D11DeviceContext *context);

private:
	
	IFW1Factory* mFW1Factory;
	IFW1FontWrapper* mFontWrapper;

	int state;
	int menuIndex;
	bool menuActive;
	wchar_t **menuItems;
	float fontSize;

	bool escapeDown;
	bool upArrowDown;
	bool downArrowDown;
	bool enterDown;
	bool tabDown;



	struct EventText
	{
		wchar_t* text;
		float timeToLive;
	};	
	std::vector<EventText> eventTexts;
};

#endif
