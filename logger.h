#include "pch.h"
#include <fstream>
#include <iostream>

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

		std::cout << message << std::endl;

		std::ofstream logFile(fileName, std::ios::out | std::ios::app);

		logFile << message << std::endl;

		logFile.close();
	}
}