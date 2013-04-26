#include "OggVorbisPlayer.h"

OggVorbisPlayer::OggVorbisPlayer()
{
	this->sBuffer = NULL;
	this->lastSection = 0;
	this->currSection = 0;
	this->almostDone = false;
	this->done = false;
	this->loop = false;
	this->initDS = false;
}

OggVorbisPlayer::OggVorbisPlayer(const OggVorbisPlayer& ovp)
{

}

OggVorbisPlayer::~OggVorbisPlayer()
{
	this->shutDownPlayer();
}

void OggVorbisPlayer::useDirectSound(IDirectSound8* _ds)
{
	this->ds = _ds;
	this->initDS = false;
}

bool OggVorbisPlayer::openOggFile(char* filePath)
{
	FILE* f;
	WAVEFORMATEX wfm;
	DSBUFFERDESC bDesc;
	HRESULT result;

	//if another song has already been loaded, deallocate
	if(this->sBuffer)
	{
		this->sBuffer->Release();
		this->sBuffer = NULL;

		if(&this->vf)
		ov_clear(&this->vf);
	}

	
		

	f = fopen(filePath, "rb");
	if(FAILED(f))
		MessageBox(0, L"", 0 , 0);

	ov_open(f, &this->vf, NULL, 0);

	vorbis_info *vi = ov_info(&this->vf, -1);
	memset(&wfm, 0, sizeof(wfm));

	wfm.cbSize = sizeof(wfm);
	wfm.nChannels = vi->channels;
	wfm.wBitsPerSample = 16;
	wfm.nSamplesPerSec = vi->rate;
	wfm.nAvgBytesPerSec = wfm.nSamplesPerSec * wfm.nChannels * 2;
	wfm.nBlockAlign = 2 * wfm.nChannels;
	wfm.wFormatTag = 1;

	bDesc.dwSize = sizeof(bDesc);
	bDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bDesc.lpwfxFormat = &wfm;
	bDesc.dwReserved = 0;
	bDesc.dwBufferBytes = BUFFERSIZE_HALF * 2;

	result = this->ds->CreateSoundBuffer(&bDesc, &this->sBuffer, NULL);
	//result = this->ds->CreateSoundBuffer(&bDesc, &tempBuffer, NULL);
	//if(FAILED(result))
	//{
	//	return false;
	//}

	//// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
	//result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*this->sBuffer);
	//if(FAILED(result))
	//{
	//	return false;
	//}

	DWORD pos = 0;
	int sec = 0;
	int ret = 1;
	DWORD size = BUFFERSIZE_HALF * 2;
	char* buff;

	this->sBuffer->Lock(0, size, (LPVOID*)&buff, &size, NULL, NULL, DSBLOCK_ENTIREBUFFER);

	while(ret && pos < size)
	{
		ret = ov_read(&this->vf, buff + pos, size - pos, 0, 2, 1, &sec);
		pos += ret;
	}

	this->sBuffer->Unlock(buff, size, NULL, NULL);

	return true;
}
void OggVorbisPlayer::close()
{}
void OggVorbisPlayer::play(bool _loop)
{
	this->sBuffer->Play(0, 0, DSBPLAY_LOOPING);

	this->loop = _loop;
	this->done = false;
	this->almostDone = false;
}

void OggVorbisPlayer::stop()
{
	this->sBuffer->Stop();
}

void OggVorbisPlayer::update()
{
	DWORD pos;
	this->sBuffer->GetCurrentPosition(&pos, NULL);

	this->currSection = pos < BUFFERSIZE_HALF?0:1;

	if(this->currSection != this->lastSection)
	{
		if(this->done && !this->loop)
		{
			this->sBuffer->Stop();
		}	

		if( this->almostDone && !this->loop)
		{
			this->done = true;
		}

		DWORD size = BUFFERSIZE_HALF;
		char* buff;
		DWORD pos1 = 0;
		int sec = 0;
		int ret = 1;

		this->sBuffer->Lock(this->lastSection*BUFFERSIZE_HALF, size, (LPVOID*)&buff, &size, NULL, NULL, 0);

		while(ret && pos1 < size)
		{
			ret = ov_read(&this->vf, buff + pos1, size - pos1, 0, 2, 1, &sec);
			pos1 += ret;
		}

		if(!ret && this->loop)
		{
			ret = 1;
			ov_pcm_seek(&this->vf, 0);
			while(ret && pos1 < size)
			{
				ret = ov_read(&this->vf, buff + pos1, size - pos1, 0, 2, 1, &sec);
				pos1 += ret;
			}
		}
		else if(!ret && !this->loop)
		{
			while(pos1 < size)
			{
				*(buff + pos1) = 0;
				pos++;
			}

			this->almostDone = true;
		}

		this->sBuffer->Unlock(buff, size, NULL, NULL);

		this->lastSection = this->currSection;
	}
}

void OggVorbisPlayer::shutDownPlayer()
{
	if(this->sBuffer != NULL)
	{
		this->sBuffer->Release();
		this->sBuffer = 0;
	}
	ov_clear(&this->vf);

	if(this->initDS)
		delete this->ds;
}

bool OggVorbisPlayer::initDirectSound(HWND hwnd)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;

	result = DirectSoundCreate8(NULL, &this->ds, NULL);
	if(FAILED(result))
	{
		return false;
	}


	result = ds->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if(FAILED(result))
	{
		return false;
	}



	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;


	result = ds->CreateSoundBuffer(&bufferDesc, &this->sBuffer, NULL);
	if(FAILED(result))
	{
		return false;
	}

	this->initDS = true;

	return true;
}

void OggVorbisPlayer::setVolume(long vlm)
{
	HRESULT r;

	r = this->sBuffer->SetVolume(vlm);
	/*if(r == DSERR_CONTROLUNAVAIL)
	{
		MessageBox(0, L"1", 0, 0);
	}
	else if(r == DS_OK)
	{
		MessageBox(0, L"no issue", 0, 0);
	}
	else if(r == DSERR_INVALIDPARAM)
	{
		MessageBox(0, L"2", 0, 0);
	}
	else if(r == DSERR_PRIOLEVELNEEDED)
	{
		MessageBox(0, L"3", 0, 0);
	}*/
}


long OggVorbisPlayer::getVolume() const
{
	long vlm = 0;

	this->sBuffer->GetVolume(&vlm);

	return vlm;
}