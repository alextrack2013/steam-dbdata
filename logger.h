#include "pch.h"
#include <fstream>
#include <iostream>

namespace logger
{
	static std::string fileName = "winmm-proxy.log";

	static void init()
	{
		std::ofstream logFile(fileName, std::ios::out);

		logFile << "Winmm Proxy" << std::endl;

		logFile.close();
	}

	static void info(std::string message)
	{
		std::cout << message << std::endl;

		std::ofstream logFile(fileName, std::ios::out | std::ios::app);

		logFile << message << std::endl;

		logFile.close();
	}
}