#ifndef SOUNDHELP_H
#define SOUNDHELP_H

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#include <vector>
#include <DirectInput.h>
#include <Camera.h>
#include <time.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <Windows.h>
#include <MMReg.h>

const int NUM_SOUNDS = 6;

enum Sound3D
{
	PlayerGrunt = 0,
	FireWeapon = 1,
	EnemyGrunt = 2,
	SwitchSound = 3,
	Running = 4,
	Jump = 5
};

const int NUM_SONGS = 3;

enum Sound
{
	Background = 0,
	Song1 = 1,
	Song2 = 2
};


#endif