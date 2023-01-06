#pragma once
#include <string>

namespace pkodev { namespace utils {

	class Path final
	{
		public:
			static std::string ExtractFileName(const std::string& path);
			static std::string ExtractFilePath(const std::string& path);
			static std::string ChangeFileExt(const std::string path, const std::string& ext);
			static std::string GetExeName();
	};

}}

