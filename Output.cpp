#include <iomanip>
#include <Windows.h>
#include "Output.h"


namespace pkodev { namespace utils { namespace output {

	std::string GetSystemDateTime()
	{
		char sysTimeStr[64] = {};
		SYSTEMTIME systemTime;

		GetLocalTime(&systemTime);

		sprintf_s(sysTimeStr,
			"%02u-%02u-%02u_%u-%02u-%02u-%03u",
			systemTime.wDay,
			systemTime.wMonth,
			systemTime.wYear,
			systemTime.wHour,
			systemTime.wMinute,
			systemTime.wSecond,
			systemTime.wMilliseconds);

		return std::string{ sysTimeStr };
	}

}}}