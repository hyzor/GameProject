//-----------------------------------------------------------------------------------------
// File: Logger.h
//
// Simple singleton log class
//-----------------------------------------------------------------------------------------

#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <fstream>

class Logger
{
public:
	static Logger* Instance();
	bool openLogFile(std::string logFile);
	void log(std::string message);
	bool closeLogFile();

	~Logger();

private:
	Logger();

	static Logger* mInstance;
	std::ofstream mFileOut;
};

#endif