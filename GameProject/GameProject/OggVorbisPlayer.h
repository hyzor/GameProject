#ifndef OGGVORBISPLAYER_H
#define OGGVORBISPLAYER_H

#pragma comment(lib, "vorbisfile_static.lib")
#pragma comment(lib, "vorbis_static.lib")
#pragma comment(lib, "ogg_static.lib")

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <Windows.h>
#include <MMReg.h>
#include <dsound.h>

const int BUFFERSIZE_HALF = 15 * 1024;

class OggVorbisPlayer
{
	private:
		int lastSection;
		int currSection;
		bool almostDone;
		bool loop;
		bool done;

		OggVorbis_File vf;
		IDirectSoundBuffer* sBuffer;
		IDirectSound8* ds;
	public:
		OggVorbisPlayer();
		OggVorbisPlayer(const OggVorbisPlayer& ovp);
		virtual ~OggVorbisPlayer();

		void useDirectSound(IDirectSound8* _ds);
		bool openOggFile(char* filePath);
		void close();
		void play(bool loop = false);
		void stop();
		void update();

		void shutDownPlayer();
		
};

#endif