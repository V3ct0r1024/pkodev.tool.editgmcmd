#pragma once
#include <string>

namespace pkodev { namespace utils {

	class String final
	{
		public:
			static std::string TrimLeft(std::string str);
			static std::string TrimRight(std::string str);
			static std::string Trim(const std::string& str);
	};

}}