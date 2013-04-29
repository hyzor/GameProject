#ifndef GAMETIMER_H
#define GAMETIMER_H

#include <Windows.h>

class GameTimer
{
public:
	GameTimer(void);
	~GameTimer(void);

	float getTimeElapsedS() const;
	float getDeltaTime() const;

	void tick();
	void start();
	void reset();
	void stop();

private:
// 	DWORD mStartTime;
// 	DWORD mCurrentTime;
// 	DWORD mPreviousTime;
// 	DWORD mPausedTime;
// 	DWORD mStopTime;

	__int64 mStartTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPreviousTime;
	__int64 mCurrentTime;

	double mSecondsPerCount;
	double mDeltaTime;

	bool mStopped;
};

#endif