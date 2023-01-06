#include <Windows.h>
#include "Path.h"


namespace pkodev { namespace utils {

	std::string Path::ExtractFileName(const std::string& path)
	{
		if (path.empty() == true) {
			return path;
		}

		const std::size_t pos = path.find_last_of("/\\");
		if (pos == path.npos) {
			return path;
		}

		return path.substr(pos + 1);
	}

	std::string Path::ExtractFilePath(const std::string& path)
	{
		if (path.empty() == true) {
			return path;
		}

		const std::size_t pos = path.find_last_of("/\\");
		if (pos == path.npos) {
			return path;
		}

		return path.substr(0, pos).append("\\");
	}

	std::string Path::ChangeFileExt(const std::string path, const std::string& ext)
	{
		if ( (path.empty() == true) || (ext.empty() == true) ) {
			return path;
		}

		const std::size_t pos = path.find_last_of('.');
		if (pos == path.npos) {
			return path;
		}

		if (ext.front() != '.') {
			return path.substr(0, pos).append(".").append(ext);
		}

		return path.substr(0, pos).append(ext);
	}

	std::string Path::GetExeName()
	{
		char filename[MAX_PATH];
		const DWORD size = GetModuleFileNameA(NULL, filename, MAX_PATH);

		if (size == 0) {
			return std::string{ "" };
		}

		return std::string{ filename };
	}

}}
