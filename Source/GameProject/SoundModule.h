#ifndef SOUNDMODULE_H
#define SOUNDMODULE_H

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")



#include "SoundHelp.h"
#include "OggVorbisPlayer.h"


// To add another sound, make another createSound()-call inside the createAllSounds()-function and //
// make necessary changes in the Sound3D/Sound enumerations and update constants(source:SoundHelp.h) //

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

		IDirectSound8* ds;
		IDirectSoundBuffer* pBuffer;
		IDirectSound3DListener8* listener;

		std::vector<SoundSource> sounds3D;
		std::vector<SoundSource> sounds;
		std::vector<IDirectSoundBuffer8*> sBuffers; //buffers to play sounds that the player generates
		std::vector<IDirectSound3DBuffer*> s3DBuffers;
		std::vector<IDirectSoundBuffer*> sEnemyBuffers; //buffers to play sounds that the enemies generates
		std::vector<IDirectSound3DBuffer8*> s3DEnemyBuffers;
		std::vector<int> IDIndices;
		std::vector<int> enemyID; // The indices in this vector are "parallell" with the indices for enemyBuffers/3DenemyBuffers

		bool SFXMuted;
		bool musicMuted;
		bool musicLoaded;
		int currSong;
		int nrOfEnemies;
		int nrOfSoundFXLoaded;
		OggVorbisPlayer ovp;
		clock_t muteTimer, musicSwapTimer;

		/////////////////////////////////////////

		bool initializeDirectSound(HWND);
		void shutDownDirectSound();
		bool loadWaveFile(char*, IDirectSoundBuffer8**, IDirectSound3DBuffer8**, int IDIndex);
		void shutDownWaveFile(IDirectSoundBuffer8**, IDirectSound3DBuffer8**);
		void createAllSounds();
		void createSound(char* path, bool is3DSound, int soundID);
		bool loadWaveFiles();
		bool initiationPlay();
		
	public:
		SoundModule();
		SoundModule(const SoundModule&);
		bool playSFX(XMFLOAT3, int soundID, bool looping = false);
		bool playDuplicateSFX(XMFLOAT3, int soundID, bool looping = false);
		bool stopSound(int sID);
		void playMusic();
		virtual ~SoundModule();
		bool initialize(HWND hwnd, DirectInput* di);
		void shutDown();
		HRESULT setSFXVolume(long vlm);
		HRESULT setMusicVolume(long vlm);
		HRESULT muteMusic(); //ditto
		HRESULT muteSFX(); //mutes SFX, call again to unmute
		HRESULT setVolume(long vlm, int SoundID);
		long getSFXVolume() const;
		long getMusicVolume() const;
		void inputGeneration(XMFLOAT3 playerPos);
		void loadMusic(int sID = 0);
		bool updateAndPlay(Camera* pCamera, XMFLOAT3 playerPos);
		bool addEnemy(int ID);
		bool playEnemySFX(int soundID, int enemyID, XMFLOAT3 pos, bool looping);
};



#endif