#include <algorithm>
#include "String.h"

namespace pkodev { namespace utils{

	std::string String::TrimLeft(std::string str)
	{
		const auto it = std::find_if(str.begin(), str.end(),
			[](const char& c) -> bool
			{
				return (std::isspace(c) == 0);
			}
		);

		str.erase(str.begin(), it);

		return str;
	}

	std::string String::TrimRight(std::string str)
	{
		const auto it = std::find_if(str.rbegin(), str.rend(),
			[](const char& c) -> bool
			{
				return (std::isspace(c) == 0);
			}
		);

		str.erase(it.base(), str.end());

		return str;
	}

	std::string String::Trim(const std::string& str)
	{
		return TrimLeft(TrimRight(str));
	}

}}