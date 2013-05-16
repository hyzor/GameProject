#include "Settings.h"

Settings* Settings::mInstance = 0;

Settings* Settings::GetInstance()
{
	if (!mInstance)
		mInstance = new Settings();

	return mInstance;
}

bool Settings::ReadFile(std::string FileName)
{
	FileStream.open(FileName);

	if (!FileStream.is_open())
		return false;

	std::string line;
	getline(FileStream, line);
	GetInstance()->mData.PlayerName = line;

	getline(FileStream, line);
	GetInstance()->mData.IP = line;

	getline(FileStream, line);
	GetInstance()->mData.Width = atoi(line.c_str());

	getline(FileStream, line);
	GetInstance()->mData.Height = atoi(line.c_str());

	getline(FileStream, line);
	GetInstance()->mData.IsFullscreen = (line.at(0) == '1');

	return true;
}

void Settings::Shutdown()
{
	if (mInstance)
	{
		delete mInstance;
		mInstance = 0;
	}
}

SettingsData Settings::GetData() const
{
	return GetInstance()->mData;
}

void Settings::SetFullscreen(bool fullscreen)
{
	GetInstance()->mData.IsFullscreen = fullscreen;
}

void Settings::SetResolution(int width, int height)
{
	GetInstance()->mData.Width = width;
	GetInstance()->mData.Height = height;
}