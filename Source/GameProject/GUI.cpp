#include "GUI.h"
#include "Vertex.h"
#include "Settings.h"

const int MENUSIZE = 4;

GUI::GUI()
{
	mFontWrapper = NULL;
	mFW1Factory = NULL;
	menuItems = NULL;
	menuActive = false;
	escapeDown = false;
	upArrowDown = false;
	downArrowDown = false;
	enterDown = false;
	menuIndex = 0;
}
GUI::~GUI()
{
	delete[]menuItems;
}
void GUI::Init(ID3D11Device *device)
{
	FW1CreateFactory(FW1_VERSION, &mFW1Factory);
	mFW1Factory->CreateFontWrapper(device, L"Consolas", &mFontWrapper);

	menuItems = new wchar_t *[MENUSIZE];

	menuItems[0] = L"Resume";
	menuItems[1] = L"Mute sounds";
	menuItems[2] = L"FullScreen";
	menuItems[3] = L"Exit";
}

bool GUI::Update(DirectInput* di)
{
	
	if(di->GetKeyboardState()[DIK_ESCAPE] & 0x80 && !escapeDown)
		escapeDown = true;

	else if(!(di->GetKeyboardState()[DIK_ESCAPE] & 0x80) && escapeDown)
	{
		if(menuActive)
		{
			menuActive = false;
		}
		else
		{
			menuActive = true;
		}
		escapeDown = false;
	}

	if(di->GetKeyboardState()[DIK_DOWN] & 0x80 && !downArrowDown)
		downArrowDown = true;
	
	else if(!(di->GetKeyboardState()[DIK_DOWN] & 0x80) && downArrowDown)
	{
		if(menuIndex < MENUSIZE-1)
		{
			menuIndex++;
		}
		else 
		{
			menuIndex = 0;
		}
		downArrowDown = false;
	}

	if(di->GetKeyboardState()[DIK_UP] & 0x80 && !upArrowDown)
		upArrowDown = true;
	
	else if(!(di->GetKeyboardState()[DIK_UP] & 0x80) && upArrowDown)
	{
		if(menuIndex > 0)
		{
			menuIndex--;
		}
		else
		{
			menuIndex = MENUSIZE-1;
		}
		upArrowDown = false;
	}

	if(di->GetKeyboardState()[DIK_RETURN] & 0x80 && !enterDown)
		enterDown = true;

	else if(!(di->GetKeyboardState()[DIK_RETURN] & 0x80) && enterDown)
	{
		if(menuActive)
		{			
			if(_wcsicmp(menuItems[menuIndex], L"Exit") == 0)
				return true;

			if(_wcsicmp(menuItems[menuIndex], L"Resume") == 0)
				menuActive = false;

			if(_wcsicmp(menuItems[menuIndex], L"FullScreen") == 0)
			{	
				bool fullscreen;
				if(Settings::GetInstance()->GetData().IsFullscreen)
					fullscreen = false;
				
				else
					fullscreen = true;

				Settings::GetInstance()->SetFullscreen(fullscreen);
			}
		}
		enterDown = false;
	}


	return false;
}

void GUI::Render(ID3D11DeviceContext *pContext)
{
	if(menuActive)
	{
		DrawMenu(pContext);
	}
	else
	{
		drawCrossHair(pContext);
	}
}

void GUI::drawCrossHair(ID3D11DeviceContext *context)
{
	int width = Settings::GetInstance()->GetData().Width;
	int heigth = Settings::GetInstance()->GetData().Height;

	drawText(context, L"-o-", XMFLOAT2(width/2, heigth/2), 25, 0xff0000ff); 
}
void GUI::drawText(ID3D11DeviceContext *context, wchar_t* text, XMFLOAT2 pos, float fontSize, int color) const
{
	mFontWrapper->DrawString(
	context,
	text,// String
	fontSize,// Font size
	pos.x,// X position
	pos.y,// Y position
	color,// Text color, 0xAaBbGgRr
	0// Flags (for example FW1_RESTORESTATE to keep context states unchanged)
	);
}

void GUI::DrawMenu(ID3D11DeviceContext *context)
{

	int unselected = 0xff990000;
	int selected = 0xffff3333;
	
	int width = Settings::GetInstance()->GetData().Width;
	int heigth = Settings::GetInstance()->GetData().Height;

	for(int i = 0; i<MENUSIZE; i++)
	{
		int color = unselected;
		if(menuIndex == i)
		{
			color = selected;
		}
		drawText(context, menuItems[i], XMFLOAT2(0.5*width, (0.4*heigth) + 60*i), 50, color);
		//drawText(context, underline, XMFLOAT2(100, 101 + 40*i));
	}
}

bool GUI::InMenu()
{
	return menuActive;
}

void GUI::Release()
{
	mFW1Factory->Release();
	mFontWrapper->Release();
}