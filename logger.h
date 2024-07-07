#include "pch.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <time.h>

namespace logger
{
	static bool is_initialized = false;
	static std::string fileName = "winmm-proxy.log";

	static void init()
	{
		std::ofstream logFile(fileName, std::ios::out);

		logFile << "Winmm Proxy" << std::endl;

		logFile.close();

		is_initialized = true;
	}

	static void info(std::string message)
	{
		if (!is_initialized)
		{
			return;
		}

		auto t = std::time(NULL);
		struct tm tm;
		localtime_s(&tm,&t);

		std::cout << std::put_time(&tm, "[%d-%m-%Y %H:%M:%S] ") << message << std::endl;

		std::ofstream logFile(fileName, std::ios::out | std::ios::app);

		logFile << std::put_time(&tm, "[%d-%m-%Y %H:%M:%S] ") << message << std::endl;

		logFile.close();
	}
}