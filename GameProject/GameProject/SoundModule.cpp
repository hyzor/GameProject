#include "SoundModule.h"

SoundModule::SoundModule()
{
	this->directSound = NULL;;
	this->primaryBuffer = NULL;
	this->listener = NULL;
	this->secondaryBackgroundBuffer = NULL;
	this->SFXMuted = false;

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

	this->setSFXVolume(DSBVOLUME_MAX);

	return result;
}

void SoundModule::shutDown()
{

	// There is one secondary 3D buffer for each secondary buffer. So one loop is possible.
	for(int i = 0; i < this->secondaryBuffers.size(); i++)
	{
		shutDownWaveFile(&(this->secondaryBuffers.at(i)), &(this->secondary3DBuffers.at(i)));
	}

	// Shutdown the Direct Sound API.
	shutDownDirectSound();
}

bool SoundModule::initializeDirectSound(HWND hwnd)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;


	// Initialize the direct sound interface pointer for the default sound device.
	result = DirectSoundCreate8(NULL, &directSound, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Set the cooperative level to priority so the format of the primary sound buffer can be modified.
	result = directSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
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
	result = directSound->CreateSoundBuffer(&bufferDesc, &primaryBuffer, NULL);
	if(FAILED(result))
	{
		return false;
	}


	// Setup the format of the primary sound bufffer.
	// In this case it is a .WAV file recorded at 44,100 samples per second in 16-bit stereo (cd audio format).
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the primary buffer to be the wave format specified.
	result = primaryBuffer->SetFormat(&waveFormat);
	if(FAILED(result))
	{
		return false;
	}


	// Obtain a listener interface.
	result = primaryBuffer->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)&this->listener);
	if(FAILED(result))
	{
		return false;
	}


	this->listener->SetPosition(0.0f, 0.0f, 0.0f, DS3D_DEFERRED);

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
	if(this->primaryBuffer)
	{
		this->primaryBuffer->Release();
		this->primaryBuffer = 0;
	}

	// Release the direct sound interface pointer.
	if(this->directSound)
	{
		this->directSound->Release();
		this->directSound = 0;
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
	error = fopen_s(&filePtr, filename/*"../../grunt_01.wav"*/, "rb");
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
	result = directSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
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
	Position standardPos;
	standardPos.x = 0.0f;
	standardPos.y = 0.0f;
	standardPos.z = 0.0f;

	createSound(/*"../../grunt_01.wav"*/ "Data/Sounds/grunt_01.wav", true, PlayerGrunt);
	createSound(/*"../../firing_weapon_03.wav*/ "Data/Sounds/firing_weapon_03.wav", true, FireWeapon);
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
	for(int i = 0; i < this->sounds3D.size(); i++)
	{	
		IDirectSoundBuffer8* SB;
		IDirectSound3DBuffer* SB3D;
		this->secondaryBuffers.push_back(SB);
		this->secondary3DBuffers.push_back(SB3D);

		result = loadWaveFile(this->sounds3D.at(i).path, &this->secondaryBuffers.at(i), &this->secondary3DBuffers.at(i), this->sounds3D.at(i).soundID);
		if(!result)
			MessageBox(0, L"Failure managing secondary buffers. Path: ", 0, 0);
	}

	return result;
}

bool SoundModule::playWaveFile(float x, float y, float z, int soundID)
{
	HRESULT result;
	
	for(int i = 0; i < this->secondaryBuffers.size(); i++)
	{
		if(soundID == this->IDIndices.at(i))
		{
			result = secondaryBuffers.at(i)->SetCurrentPosition(0);
			if(FAILED(result))
			{
				return false;
			}

			result = secondary3DBuffers.at(i)->SetPosition(x, y, z, DS3D_DEFERRED);
			if(FAILED(result))
			{
				return false;
			}

			result = secondary3DBuffers.at(i)->SetMinDistance(20.0f, DS3D_DEFERRED);
			if(FAILED(result))
			{
				return false;
			}

			this->listener->CommitDeferredSettings();

			result = secondaryBuffers.at(i)->Play(0, 0, 0);
			if(FAILED(result))
			{
				return false;
			}
		}
	}

	return true;
}

void SoundModule::playSound(float x, float y, float z, int soundID)
{
	bool result = playWaveFile(x, y, z, soundID);
	if(!result)
	{
		MessageBox(0, L"Failing playing sound.", 0, 0);
	}
}

void SoundModule::setListenerPosition(float x, float y, float z)
{
	this->listener->SetPosition(x,y,z, DS3D_DEFERRED);
}

HRESULT SoundModule::setSFXVolume(long vlm)
{
	HRESULT result = true;

	if(vlm > DSBVOLUME_MAX)
		vlm = DSBVOLUME_MAX;
	else if(vlm < DSBVOLUME_MIN)
		vlm = DSBVOLUME_MIN;

	for(int i = 0; i < this->secondaryBuffers.size(); i++)
	{
		result = this->secondaryBuffers.at(i)->SetVolume(vlm);
		if(FAILED(result))
			return false;
	}
	return result;
}

HRESULT SoundModule::setMusicVolume(long vlm)
{
	HRESULT result = true;

	if(vlm > DSBVOLUME_MAX)
		vlm = DSBVOLUME_MAX;
	else if(vlm < DSBVOLUME_MIN)
		vlm = DSBVOLUME_MIN;

	result = this->secondaryBackgroundBuffer->SetVolume(vlm);

	return result;
}

HRESULT SoundModule::muteSFX()
{
	HRESULT result = true;

	for(int i = 0; i < this->secondaryBuffers.size(); i++)
	{
		if(this->SFXMuted)
		{
			result = this->secondaryBuffers.at(i)->SetVolume(DSBVOLUME_MAX);
			if(FAILED(result))
				return false;
		}
		else
		{
			result = this->secondaryBuffers.at(i)->SetVolume(DSBVOLUME_MIN);
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

	for(int i = 0; i < this->secondaryBuffers.size(); i++)
	{
		if(SoundID == (this->IDIndices.at(i)))
		{
			result = this->secondaryBuffers.at(i)->SetVolume(vlm);
			if(FAILED(result))
				return false;
		}
	}
	return result;
}

long SoundModule::getSFXVolume() const
{
	long averageVlm = 0;
	long vlm = 0;
	int nrOfSounds = 0;

	for(int i = 0; i < this->secondaryBuffers.size(); i++)
	{
		this->secondaryBuffers.at(i)->GetVolume(&vlm);
		averageVlm += vlm;
		nrOfSounds++;
	}

	averageVlm = (averageVlm / nrOfSounds);

	return averageVlm;
}

long SoundModule::getMusicVolume() const
{
	long vlm;

	this->secondaryBackgroundBuffer->GetVolume(&vlm);

	return vlm;
}

void SoundModule::inputGeneration(float x, float y, float z)
{
	long currentVolume = 0;
	if(this->dInput->GetKeyboardState()[DIK_M] && 0x80 )
	{
		this->muteSFX();
	}
	if((this->dInput->GetMouseState().rgbButtons[0] && 0x80 ))
	{
		this->playSound(x, y, z, FireWeapon);
	}
	if(this->dInput->GetKeyboardState()[DIK_ADD] && 0x80 )
	{
		currentVolume = this->getSFXVolume();
		this->setSFXVolume(currentVolume + 500);
	}
	if(this->dInput->GetKeyboardState()[DIK_SUBTRACT] && 0x80 )
	{
		currentVolume = this->getSFXVolume();
		this->setSFXVolume(currentVolume - 500);
	}
	if(this->dInput->GetKeyboardState()[DIK_NUMPAD1] && 0x80)
	{
		this->playSound(0,0,0, PlayerGrunt);
	}
}