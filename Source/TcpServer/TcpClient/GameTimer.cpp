#include "GameTimer.h"


GameTimer::GameTimer(void)
	: mSecondsPerCount(0.0), mDeltaTime(-1.0), mStartTime(0), 
	mPausedTime(0), mPreviousTime(0), mCurrentTime(0), mStopped(false)
{
// 	DWORD countsPerSec;
// 	countsPerSec = GetTickCount();
// 	mSecondsPerCount = 1.0 / (double)countsPerSec;

	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
}

void GameTimer::start()
{
// 	DWORD startTime;
// 	startTime = GetTickCount();

	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if (mStopped)
	{
		mPausedTime += (startTime - mStopTime);

		mPreviousTime = startTime;
		mStopTime = 0;
		mStopped = false;
	}
}

void GameTimer::reset()
{
	//DWORD curTime = GetTickCount();

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mStartTime = currTime;
	mPreviousTime = currTime;
	mStopTime = 0;
	mStopped = false;
}

GameTimer::~GameTimer(void)
{
}

float GameTimer::getTimeElapsedS() const
{
	if (mStopped)
	{
		return (float)(((mStopTime - mPausedTime)-mStartTime)*mSecondsPerCount);
	}
	else
	{
		return (float)(((mCurrentTime-mPausedTime)-mStartTime)*mSecondsPerCount);
	}
}

void GameTimer::tick()
{
	if (mStopped)
	{
		mDeltaTime = 0.0;
		return;
	}

// 	DWORD curTime;
// 	curTime = GetTickCount();

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrentTime = currTime;

	mDeltaTime = (currTime - mPreviousTime) * mSecondsPerCount;

	mPreviousTime = currTime;

	if (mDeltaTime < 0.0)
	{
		mDeltaTime = 0.0;
	}
}

float GameTimer::getDeltaTime() const
{
	return (float)mDeltaTime;
}

void GameTimer::stop()
{
	if (!mStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		//mStopTime = GetTickCount();
		mStopTime = currTime;
		mStopped = true;
	}
}
