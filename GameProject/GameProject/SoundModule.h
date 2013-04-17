#ifndef SOUNDMODULE_H
#define SOUNDMODULE_H

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include "SoundHelp.h"
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#include <vector>
#include <DirectInput.h>
#include <Camera.h>


struct Position
{
	float x;
	float y;
	float z;
};

// To add another sound, make another createSound()-call inside the createAllSounds()-function and //
// make necessary changes in the Sound3D/Sound enumerations (source:stdafx.h) //

class SoundModule
{
	private:
		struct dotWavHeaderType
		{
			char chunkId[4];
			unsigned long chunkSize;
			char format[4];
			char subChunkId[4];
			unsigned long subChunkSize;
			unsigned short audioFormat;
			unsigned short numChannels;
			unsigned long sampleRate;
			unsigned long bytesPerSecond;
			unsigned short blockAlign;
			unsigned short bitsPerSample;
			char dataChunkId[4];
			unsigned long dataSize;
		};

		struct SoundSource
		{
			char* path;
			int soundID;
			bool sound3D;
		};

		DirectInput* dInput;

		IDirectSound8* directSound;
		IDirectSoundBuffer* primaryBuffer;
		IDirectSound3DListener8* listener;

		IDirectSoundBuffer8* secondaryBackgroundBuffer;
		std::vector<SoundSource> sounds3D;
		std::vector<SoundSource> sounds;
		std::vector<IDirectSoundBuffer8*> secondaryBuffers; 
		std::vector<IDirectSound3DBuffer*> secondary3DBuffers;
		std::vector<int> IDIndices;

		bool SFXMuted;

		/////////////////////////////////////////

		bool initializeDirectSound(HWND);
		void shutDownDirectSound();
		bool loadWaveFile(char*, IDirectSoundBuffer8**, IDirectSound3DBuffer8**, int IDIndex);
		void shutDownWaveFile(IDirectSoundBuffer8**, IDirectSound3DBuffer8**);
		bool playWaveFile(float x, float y, float z, int soundID );
		void createAllSounds();
		void createSound(char*, bool is3DSound, int soundID);
		bool loadWaveFiles();
		HRESULT initiationPlay();


	public:
		SoundModule();
		SoundModule(const SoundModule&);
		void setListenerPosition(float x, float y, float z);
		void setSoundPosition(float x, float y, float z);
		void playSound(float x, float y, float z, int soundID);
		virtual ~SoundModule();
		bool initialize(HWND hwnd, DirectInput* di);
		void shutDown();
		HRESULT setSFXVolume(long vlm);
		HRESULT setMusicVolume(long vlm);
		HRESULT muteSFX(); //mutes SFX, call again to unmute
		HRESULT setVolume(long vlm, int SoundID);
		HRESULT getSFXVolume() const;
		long getMusicVolume() const;
		void inputGeneration(float x, float y, float z);
};



#endif