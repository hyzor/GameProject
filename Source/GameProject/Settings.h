#ifndef SETTINGS_H
#define SETTINGS_H

//#include <d3dUtilities.h>
#include <string>
#include <fstream>
#include <sstream>

struct SettingsData
{
	int Width, Height;
	std::string IP;
	std::string PlayerName;
	bool IsFullscreen;
};

class Settings
{
public:
	static Settings* GetInstance();

	bool ReadFile(std::string FileName);

	SettingsData GetData() const;

	void Shutdown();

private:
	Settings() {};
	static Settings* mInstance;

	SettingsData mData;

	std::fstream FileStream;
};

#endif