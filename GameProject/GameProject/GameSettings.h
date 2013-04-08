#ifndef GAMESETTINGS_H_
#define GAMESETTINGS_H_

#include <d3dUtilities.h>

struct FogSettings
{
	float fogStart;
	float fogRange;
	bool fogEnabled;
};

class GameSettings
{
public:
	static GameSettings* Instance();
	~GameSettings();

	FogSettings* Fog();

protected:
	void defaultSettings();

private:
	GameSettings();

	static GameSettings* mInstance;

	FogSettings mFogSettings;
};

#endif