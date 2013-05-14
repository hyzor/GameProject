#include "SoundModule.h"

SoundModule::SoundModule()
{
	this->ds = NULL;;
	this->pBuffer = NULL;
	this->listener = NULL;
	this->SFXMuted = false;
	this->musicMuted = false;
	this->musicLoaded = false;
	this->currSong = 0;
	this->muteTimer = clock();
	this->musicSwapTimer = clock();
	this->nrOfEnemies = 0;
	this->nrOfSoundFXLoaded = 0;
}

SoundModule::SoundModule(const SoundModule& SM)
{


}

SoundModule::~SoundModule()
{
	this->shutDown();
}

bool SoundModule::initialize(HWND hwnd, DirectInput* di)
{
	bool result;

	this->dInput = di;

	// Initialize direct sound and the primary sound buffer.
	result = initializeDirectSound(hwnd);
	if(!result)
	{
		MessageBox(0, L"Failing initializing DirectSound.", 0, 0);
		return false;
	}

	createAllSounds();

	result = loadWaveFiles();
	if(!result)
	{
		MessageBox(0, L"Failing loading wave files into secondary buffers.", 0, 0);
		return false;
	}

	this->listener->CommitDeferredSettings();

	result = this->initiationPlay(); //In order to make DirectSound create a primarybuffer (it does this during the first play()-call)
	
	this->setSFXVolume(DSBVOLUME_MAX);

	//this->ovp.useDirectSound(this->ds);
	this->ovp.initDirectSound(hwnd);
	return result;
}

void SoundModule::shutDown()
{

	// There is one secondary 3D buffer for each secondary buffer. So one loop is possible.
	for(unsigned int i = 0; i < this->sBuffers.size(); i++)
	{
		shutDownWaveFile(&(this->sBuffers.at(i)), &(this->s3DBuffers.at(i)));
	}

	//this->ovp.stop();
	this->ovp.shutDownPlayer(); 

	// Shutdown the Direct Sound API.
	shutDownDirectSound();
}

bool SoundModule::initializeDirectSound(HWND hwnd)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;


	// Initialize the direct sound interface pointer for the default sound device.
	result = DirectSoundCreate8(NULL, &ds, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Set the cooperative level to priority so the format of the primary sound buffer can be modified.
	result = ds->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if(FAILED(result))
	{
		return false;
	}


	// Setup the primary buffer description.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Get control of the primary sound buffer on the default sound device.
	result = ds->CreateSoundBuffer(&bufferDesc, &pBuffer, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Obtain a listener interface.
	result = pBuffer->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)&this->listener);
	if(FAILED(result))
	{
		return false;
	}


	this->listener->SetPosition(0,200,50, DS3D_DEFERRED);
	this->listener->SetOrientation(0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, DS3D_DEFERRED);

	return true;
}

void SoundModule::shutDownDirectSound()
{

	// Release the listener interface.
	if(this->listener)
	{
		this->listener->Release();
		this->listener = 0;
	}

	// Release the primary sound buffer pointer.
	if(this->pBuffer)
	{
		this->pBuffer->Release();
		this->pBuffer = 0;
	}

	// Release the direct sound interface pointer.
	if(this->ds)
	{
		this->ds->Release();
		this->ds = 0;
	}
}

bool SoundModule::loadWaveFile(char* filename, IDirectSoundBuffer8** secondaryBuffer, IDirectSound3DBuffer8** secondary3DBuffer, int IDIndex)
{
	int error;
	FILE* filePtr;
	unsigned int count;
	dotWavHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT result;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* waveData;
	unsigned char* bufferPtr;
	unsigned long bufferSize;


	// Open the wave file in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if(error != 0)
	{
		MessageBox(0, L"here", 0, 0);
		return false;
	}

	// Read in the wave file header.
	static_cast<unsigned>(count) = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Check that the chunk ID is the RIFF format.
	if((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') || 
	   (waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	// Check that the file format is the WAVE format.
	if((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
	   (waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	// Check that the sub chunk ID is the fmt format.
	if((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
	   (waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
	{
		return false;
	}

	// Check that the audio format is WAVE_FORMAT_PCM.
	if(waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
	{
		return false;
	}

	// Check that the wave file was recorded in mono format.
	if(waveFileHeader.numChannels != 1)
	{
		return false;
	}

	// Check that the wave file was recorded at a sample rate of 44.1 KHz.
	if(waveFileHeader.sampleRate != 44100)
	{
		return false;
	}

	// Ensure that the wave file was recorded in 16 bit format.
	if(waveFileHeader.bitsPerSample != 16)
	{
		return false;
	}

	// Check for the data chunk header.
	if((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
	   (waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
	{
		return false;
	}

	// Set the wave format of secondary buffer that this wave file will be loaded onto.
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 1;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Create a temporary sound buffer with the specific buffer settings.
	result = ds->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the temporary buffer.
	tempBuffer->Release();
	tempBuffer = 0;

	// Move to the beginning of the wave data which starts at the end of the data chunk header.
	fseek(filePtr, sizeof(dotWavHeaderType), SEEK_SET);

	// Create a temporary buffer to hold the wave file data.
	waveData = new unsigned char[waveFileHeader.dataSize];
	if(!waveData)
	{
		return false;
	}

	// Read in the wave file data into the newly created buffer.
	static_cast<unsigned>(count)  = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
	if(count != waveFileHeader.dataSize)
	{
		return false;
	}

	// Close the file once done reading.
	error = fclose(filePtr);
	if(error != 0)
	{
		return false;
	}

	// Lock the secondary buffer to write wave data into it.
	result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if(FAILED(result))
	{
		return false;
	}

	// Copy the wave data into the buffer.
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	// Unlock the secondary buffer after the data has been written to it.
	result = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if(FAILED(result))
	{
		return false;
	}
	
	// Release the wave data since it was copied into the secondary buffer.
	delete [] waveData;
	waveData = 0;

	// Get the 3D interface to the secondary sound buffer.
	result = (*secondaryBuffer)->QueryInterface(IID_IDirectSound3DBuffer8, (void**)&*secondary3DBuffer);
	if(FAILED(result))
	{
		return false;
	}

	(*secondary3DBuffer)->SetMode(DS3DMODE_NORMAL, DS3D_DEFERRED);
	this->IDIndices.push_back(IDIndex);


	return true;
}

void SoundModule::shutDownWaveFile(IDirectSoundBuffer8** secondaryBuffer, IDirectSound3DBuffer8** secondary3DBuffer)
{

	// Release the 3D interface to the secondary sound buffer.
	if(*secondary3DBuffer)
	{
		(*secondary3DBuffer)->Release();
		*secondary3DBuffer = 0;
	}

	// Release the secondary sound buffer.
	if(*secondaryBuffer)
	{
		(*secondaryBuffer)->Release();
		*secondaryBuffer = 0;
	}
}

void SoundModule::createAllSounds()
{
	createSound("Data/Sounds/grunt_01.wav", true, PlayerGrunt);
	createSound("Data/Sounds/firing_weapon_03.wav", true, FireWeapon);
	createSound("Data/Sounds/running.wav", true, Running);

	createSound("Data/Sounds/14_Aerials.ogg", false, Song1);
	createSound("Data/Sounds/Storaged.ogg", false, Song2);
}

void SoundModule::createSound(char* path,  bool is3DSound, int ID)
{
	SoundSource sound;
	sound.sound3D = is3DSound;
	sound.path = path;
	sound.soundID = ID;

	if(is3DSound)
	{
		this->sounds3D.push_back(sound);
	}
	else
	{
		this->sounds.push_back(sound);
	}

}

bool SoundModule::loadWaveFiles()
{
	bool result;
	for(unsigned int i = 0; i < this->sounds3D.size(); i++)
	{	
		IDirectSoundBuffer8* SB;
		IDirectSound3DBuffer* SB3D;
		this->sBuffers.push_back(SB);
		this->s3DBuffers.push_back(SB3D);

		result = loadWaveFile(this->sounds3D.at(i).path, &this->sBuffers.at(i), &this->s3DBuffers.at(i), this->sounds3D.at(i).soundID);
		if(!result)
			MessageBox(0, L"Failure managing secondary buffers.", 0, 0);
		this->nrOfSoundFXLoaded++;
	}

	return result;
}

bool SoundModule::playSFX(XMFLOAT3 pos, int sID, bool looping)
{
	HRESULT result;
	DWORD status;

	for(unsigned int i = 0; i < this->sBuffers.size(); i++)
	{
		if(sID == this->IDIndices.at(i))
		{
			sBuffers.at(i)->GetStatus(&status);

			if(!(status == DSBSTATUS_PLAYING))
			{

				result = sBuffers.at(i)->SetCurrentPosition(0);
				if(FAILED(result))
				{
					return false;
				}

				result = s3DBuffers.at(i)->SetMinDistance(20.0f, DS3D_DEFERRED);
				if(FAILED(result))
				{
					return false;
				}
				
				result = s3DBuffers.at(i)->SetPosition(pos.x, pos.y, pos.z, DS3D_DEFERRED);
				if(FAILED(result))
				{
					return false;
				}

				this->listener->CommitDeferredSettings();

				if(!looping)
				{
					result = sBuffers.at(i)->Play(0, 0, 0);
					if(FAILED(result))
					{
						return false;
					}
				}
				else if(looping)
				{
					result = sBuffers.at(i)->Play(0, 0, DSBPLAY_LOOPING);
					if(FAILED(result))
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

bool SoundModule::playDuplicateSFX(XMFLOAT3 pos, int sID, bool looping)
{
	HRESULT result;
	DWORD status;
	IDirectSoundBuffer* duplicate = NULL;
	IDirectSound3DBuffer8* duplicate3D = NULL;

	for(unsigned int i = 0; i < this->sBuffers.size(); i++)
	{
		if(sID == this->IDIndices.at(i))
		{
			this->sBuffers.at(i)->GetStatus(&status);
			if(!(status == DSBSTATUS_PLAYING))
			{
				result = sBuffers.at(i)->SetCurrentPosition(0);
				if(FAILED(result))
				{
					return false;
				}

				result = s3DBuffers.at(i)->SetPosition(pos.x, pos.y, pos.z, DS3D_DEFERRED);
				if(FAILED(result))
				{
					return false;
				}

				result = s3DBuffers.at(i)->SetMinDistance(20.0f, DS3D_DEFERRED);
				if(FAILED(result))
				{
					return false;
				}

				this->listener->CommitDeferredSettings();

				if(!looping)
				{
					result = sBuffers.at(i)->Play(0, 0, 0);
					if(FAILED(result))
					{
						return false;
					}
				}
				else if(looping)
				{
					result = sBuffers.at(i)->Play(0, 0, DSBPLAY_LOOPING);
					if(FAILED(result))
					{
						return false;
					}
				}
			}
			else
			{
				result = this->ds->DuplicateSoundBuffer(this->sBuffers.at(i), &duplicate);
				if(FAILED(result))
				{
					return false;
				}

				result = (duplicate)->QueryInterface(IID_IDirectSound3DBuffer, (void**)&duplicate3D);
				if(FAILED(result))
				{
					return false;
				}

				result = duplicate->SetCurrentPosition(0);
				if(FAILED(result))
				{
					return false;
				}

				long vlm;
				result = duplicate->GetVolume(&vlm);
				if(FAILED(result))
				{
					return false;
				}

				if(vlm > DSBVOLUME_MIN)
				{
					result = duplicate->SetVolume(vlm - 1);
					if(FAILED(result))
					{
						return false;
					}
				}
				else if( vlm <= DSBVOLUME_MIN)
				{
					result = duplicate->SetVolume(vlm + 1);
					if(FAILED(result))
					{
						return false;
					}
				}
				
				result = duplicate3D->SetPosition(pos.x, pos.y, pos.z, DS3D_DEFERRED);
				if(FAILED(result))
				{
					return false;
				}

				result = duplicate3D->SetMinDistance(20.0f, DS3D_DEFERRED);
				if(FAILED(result))
				{
					return false;
				}

				this->listener->CommitDeferredSettings();

				if(!looping)
				{
					result = duplicate->Play(0, 0, 0);
					if(FAILED(result))
					{
						return false;
					}
				}
				else if(looping)
				{
					result = duplicate->Play(0, 0, DSBPLAY_LOOPING);
					if(FAILED(result))
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

HRESULT SoundModule::setSFXVolume(long vlm)
{
	HRESULT result = true;

	if(vlm > DSBVOLUME_MAX)
		vlm = DSBVOLUME_MAX;
	else if(vlm < DSBVOLUME_MIN)
		vlm = DSBVOLUME_MIN;

	for(unsigned int i = 0; i < this->sBuffers.size(); i++)
	{
		result = this->sBuffers.at(i)->SetVolume(vlm);
		if(FAILED(result))
			return false;
	}
	return result;
}

HRESULT SoundModule::setMusicVolume(long vlm)
{
	HRESULT result = true;

	if(this->musicLoaded)
	{
		if(vlm > DSBVOLUME_MAX)
			vlm = DSBVOLUME_MAX;
		else if(vlm < DSBVOLUME_MIN)
			vlm = DSBVOLUME_MIN;

		this->ovp.setVolume(vlm);
	}	

	return result;
}

HRESULT SoundModule::muteSFX()
{
	HRESULT result = true;

	for(unsigned int i = 0; i < this->sBuffers.size(); i++)
	{
		if(this->SFXMuted)
		{
			result = this->sBuffers.at(i)->SetVolume(DSBVOLUME_MAX);
			if(FAILED(result))
				return false;
		}
		else
		{
			result = this->sBuffers.at(i)->SetVolume(DSBVOLUME_MIN);
			if(FAILED(result))
				return false;
		}
	}
	if(this->SFXMuted)
		this->SFXMuted = false;
	else
		this->SFXMuted = true;
	return result;
}

HRESULT SoundModule::setVolume(long vlm, int SoundID)
{
	HRESULT result = true;

	if(vlm > DSBVOLUME_MAX)
		vlm = DSBVOLUME_MAX;
	else if(vlm < DSBVOLUME_MIN)
		vlm = DSBVOLUME_MIN;

	for(unsigned int i = 0; i < this->sBuffers.size(); i++)
	{
		if(SoundID == (this->IDIndices.at(i)))
		{
			result = this->sBuffers.at(i)->SetVolume(vlm);
			if(FAILED(result))
				return false;
		}
	}

	if(this->musicLoaded)
		this->ovp.setVolume(vlm);

	return result;
}

long SoundModule::getSFXVolume() const
{
	long averageVlm = 0;
	long vlm = 0;
	int nrOfSounds = 0;

	for(unsigned int i = 0; i < this->sBuffers.size(); i++)
	{
		this->sBuffers.at(i)->GetVolume(&vlm);
		averageVlm += vlm;
		nrOfSounds++;
	}

	averageVlm = (averageVlm / nrOfSounds);

	return averageVlm;
}

long SoundModule::getMusicVolume() const
{
	long vlm = 0;
	if(this->musicLoaded)
		vlm = this->ovp.getVolume();

	return vlm;
}

void SoundModule::inputGeneration(XMFLOAT3 playerPos)
{
	long currentVolume = 0;
	D3DVECTOR listenPos;

	this->listener->GetPosition(&listenPos);

	if(this->dInput->GetKeyboardState()[DIK_M] && 0x80 )
	{
		clock_t curTime = clock();
		if((curTime - this->muteTimer) > 500 )
		{
			this->muteSFX();
			this->muteMusic();
			this->muteTimer = clock();
		}
	}

	/*
	if((this->dInput->GetMouseState().rgbButtons[0] && 0x80 ))
	{
		this->playSFX(playerPos, FireWeapon);
	}
	*/

	if(this->dInput->GetKeyboardState()[DIK_ADD] && 0x80 )
	{
		currentVolume = this->getSFXVolume();
		this->setSFXVolume(currentVolume + 125);

		currentVolume = 0;
		currentVolume = this->getMusicVolume();
		this->setMusicVolume(currentVolume + 125);
	}

	if(this->dInput->GetKeyboardState()[DIK_SUBTRACT] && 0x80 )
	{
		currentVolume = this->getSFXVolume();
		this->setSFXVolume(currentVolume - 125);

		currentVolume = 0;
		currentVolume = this->getMusicVolume();
		this->setMusicVolume(currentVolume - 125);
	}

	if(this->dInput->GetKeyboardState()[DIK_NUMPAD1] && 0x80)
	{	
		this->playDuplicateSFX(playerPos, PlayerGrunt);
	}

	if(this->dInput->GetKeyboardState()[DIK_N] && 0x80)
	{
		clock_t curTime = clock();
		if((curTime - this->musicSwapTimer) > 500)
		{
			this->loadMusic(this->currSong);
			this->playMusic();
			this->currSong++;

			if(this->currSong == NUM_SONGS)
				this->currSong = 0;

			this->musicSwapTimer = clock();
		}
	}

}

bool SoundModule::stopSound(int sID)
{
	HRESULT result;
	DWORD status;

	for(unsigned int i = 0; i < this->sBuffers.size(); i++)
	{
		if(sID == this->IDIndices.at(i))
		{
			
			result = this->sBuffers.at(i)->GetStatus(&status);
			if(FAILED(result))
				return false;

			if((status == DSBSTATUS_PLAYING))
			{
				result = this->sBuffers.at(i)->Stop();
				if(FAILED(result))
				return false;

				result = this->sBuffers.at(i)->SetCurrentPosition(0);
				if(FAILED(result))
				return false;
			}
			
		}
	}

	return true;
}

bool SoundModule::initiationPlay()
{
	HRESULT result;

	result = this->sBuffers.at(0)->SetCurrentPosition(0);
	if(FAILED(result))
		MessageBox(0, L"here",0,0);
	
	result = this->sBuffers.at(0)->SetVolume(-9999);
	if(FAILED(result))
		MessageBox(0, L"here",0,0);
	
	result = s3DBuffers.at(0)->SetPosition(0, 0, 0, DS3D_DEFERRED);
	if(FAILED(result))
	{
		return false;
	}

	result = this->listener->CommitDeferredSettings();
	if(FAILED(result))
	{
		return false;
	}

	result = this->sBuffers.at(0)->Play(0,0,0);
	if(FAILED(result))
		MessageBox(0, L"here",0,0);

	result = this->sBuffers.at(0)->Stop();
	if(FAILED(result))
		MessageBox(0, L"here",0,0);

	return true;
}

void SoundModule::playMusic()
{
	if(this->musicLoaded)
		this->ovp.play(true);
}

void SoundModule::loadMusic(int sID)
{
	for(unsigned int i = 0; i < this->sounds.size(); i++)
	{
		if(sID == this->sounds.at(i).soundID)
		{
			this->ovp.openOggFile(this->sounds.at(i).path);
			this->musicLoaded = true;
			long vlm = 0;
			vlm = this->getSFXVolume();
			this->setMusicVolume(vlm);
		}
	}
}

HRESULT SoundModule::muteMusic()
{
	if(this->musicLoaded)
	{
		if(!(this->musicMuted))
		{
			this->ovp.setVolume(DSBVOLUME_MIN);
		}
		else
		{
			this->ovp.setVolume(DSBVOLUME_MAX);
		}
	

		if(this->musicMuted)
			this->musicMuted = false;
		else
			this->musicMuted = true;
	}
	return true;
}

bool SoundModule::updateAndPlay(Camera* pCamera, XMFLOAT3 pPos)
{
	XMFLOAT3 pos;
	XMFLOAT3 up, look;
	HRESULT result;

	pos = pPos;
	result = this->listener->SetPosition(pos.x,pos.y,pos.z, DS3D_DEFERRED);
	if(FAILED(result))
		return false;

	up = pCamera->GetUp();
	look = pCamera->GetLook();
	
	result = this->listener->SetOrientation(look.x, look.y, look.z, up.x, up.y, up.z, DS3D_DEFERRED);
	if(FAILED(result))
		return false;

	for(unsigned int i = 0; i < this->s3DBuffers.size(); i++)
	{
		this->s3DBuffers.at(i)->SetPosition(pos.x, pos.y, pos.z, DS3D_DEFERRED);
	}

	this->listener->CommitDeferredSettings();

	this->inputGeneration(pos);

	if(this->musicLoaded)
		this->ovp.update();

	return true;
}

bool SoundModule::addEnemy(int ID)
{
	HRESULT result;

	for(unsigned int i = 0; i < this->sBuffers.size(); i++)
	{
		IDirectSoundBuffer* enemyBuffer;
		IDirectSound3DBuffer8* enemy3DBuffer;

		this->ds->DuplicateSoundBuffer(this->sBuffers.at(i), &enemyBuffer);
		result = (enemyBuffer)->QueryInterface(IID_IDirectSound3DBuffer, (void**)&enemy3DBuffer);
		if(FAILED(result))
		{
			return false;
		}

		this->sEnemyBuffers.push_back(enemyBuffer);
		this->s3DEnemyBuffers.push_back(enemy3DBuffer);

	}

	this->enemyID.push_back(ID);

	return true;
}

bool SoundModule::playEnemySFX(int soundID, int enemyID, XMFLOAT3 pos, bool looping)
{
	HRESULT result;
	DWORD status;
	int index = enemyID - 1;

	for(unsigned int i = 0; i < this->sEnemyBuffers.size(); i++)
	{
		if(soundID == this->IDIndices.at(i))
		{
			sEnemyBuffers.at(i * index)->GetStatus(&status);

			if(!(status == DSBSTATUS_PLAYING))
			{

				result = sEnemyBuffers.at(i * index)->SetCurrentPosition(0);
				if(FAILED(result))
				{
					return false;
				}

				result = s3DEnemyBuffers.at(i * index)->SetMinDistance(20.0f, DS3D_DEFERRED);
				if(FAILED(result))
				{
					return false;
				}
				
				result = s3DEnemyBuffers.at(i * index)->SetPosition(pos.x, pos.y, pos.z, DS3D_DEFERRED);
				if(FAILED(result))
				{
					return false;
				}

				this->listener->CommitDeferredSettings();

				if(!looping)
				{
					result = sEnemyBuffers.at(i * index)->Play(0, 0, 0);
					if(FAILED(result))
					{
						return false;
					}
				}
				else if(looping)
				{
					result = sEnemyBuffers.at(i * index)->Play(0, 0, DSBPLAY_LOOPING);
					if(FAILED(result))
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}