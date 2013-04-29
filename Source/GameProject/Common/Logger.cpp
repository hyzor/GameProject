//-----------------------------------------------------------------------------------------
// File: Logger.cpp
//
// Simple singleton log class
//-----------------------------------------------------------------------------------------

#include "Logger.h"
#include <time.h>

Logger* Logger::mInstance = NULL;


Logger::Logger(void)
{
}

Logger::~Logger(void)
{
	closeLogFile();
}

Logger* Logger::Instance()
{
	if (!mInstance)
		mInstance = new Logger();

	return mInstance;
}

bool Logger::openLogFile(std::string logFile)
{
	if (mFileOut.is_open())
		return false;

	mFileOut.open(logFile);

	time_t timer = time(0);
	//struct tm* now = localtime(&timer);

	struct tm now;
	localtime_s(&now, &timer);

	mFileOut << "Log started on " 
		<< (now.tm_year + 1900) << "-"
		<< (now.tm_mon + 1) << "-"
		<< now.tm_mday << " "
		<< now.tm_hour << ":"
		<< now.tm_min
		<< "\n\n";


	return true;
}

void Logger::log(std::string message)
{
	if (mFileOut.is_open())
	{
		mFileOut << message << "\n";
	}
}

bool Logger::closeLogFile()
{
	if (!mFileOut.is_open())
		return false;

	mFileOut << "\nLogging ended.";
	mFileOut.close();
	return true;
}