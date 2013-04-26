#ifndef OGGVORBISPLAYER_H
#define OGGVORBISPLAYER_H

#pragma comment(lib, "vorbisfile_static.lib")
#pragma comment(lib, "vorbis_static.lib")
#pragma comment(lib, "ogg_static.lib")

#include "SoundHelp.h"
const int BUFFERSIZE_HALF = 15 * 1024;

class OggVorbisPlayer
{
	private:
		int lastSection;
		int currSection;
		bool almostDone;
		bool loop;
		bool done;
		bool initDS;

		OggVorbis_File vf;
		IDirectSoundBuffer* sBuffer;
		IDirectSound8* ds;
	public:
		OggVorbisPlayer();
		OggVorbisPlayer(const OggVorbisPlayer& ovp);
		virtual ~OggVorbisPlayer();

		bool initDirectSound(HWND);
		void useDirectSound(IDirectSound8* _ds);
		bool openOggFile(char* filePath);
		void close();
		void play(bool loop = false);
		void stop();
		void update();
		void setVolume(long vlm);
		long getVolume() const;

		void shutDownPlayer();
		
};

#endif