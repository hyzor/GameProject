#include "GameSettings.h"

GameSettings* GameSettings::mInstance = NULL;

GameSettings* GameSettings::Instance()
{
	if (!mInstance)
	{
		mInstance = new GameSettings();
		mInstance->defaultSettings();
	}

	return mInstance;
}


GameSettings::GameSettings(void)
{
}


GameSettings::~GameSettings(void)
{
}

void GameSettings::defaultSettings()
{
	Instance()->mFogSettings.fogEnabled = true;
	Instance()->mFogSettings.fogStart = 40.0f;
	Instance()->mFogSettings.fogRange = 400.0f;
}

FogSettings* GameSettings::Fog()
{
	return &mFogSettings;
}
