#include <iostream>
#include <iomanip>

#include "SupportedCommand.h"


namespace pkodev {

	std::string CSupportedCommand::GetName() const
	{
		return std::string{ "supported" };
	}

	std::string CSupportedCommand::GetDescription() const
	{
		return std::string{ "Print a list of supported .exe files as a table." };
	}

	std::string CSupportedCommand::Usage() const
	{
		return std::string{ GetName() + CAddressRequiredCommand::Usage() };
	}

	ICommand::RequiredArgs CSupportedCommand::GetRequiredArgs() const
	{
		return CAddressRequiredCommand::GetRequiredArgs();
	}

	ICommand::ErrorCode CSupportedCommand::Execute(const Args& args)
	{
		ErrorCode ret = CAddressRequiredCommand::Execute(args);
		if (ret != ErrorCode::Ok) {
			return ret;
		}

		std::cout << '+' << std::setfill('-') << std::setw(7) << '+' << std::setw(49) << '+' << std::setw(18) << '+' << std::setw(19) << '+' << '\n';
		std::cout << '|' << std::setfill(' ') << std::setw(6) << "# " << '|' << std::setw(48) << "Executable file name " << '|' << std::setw(17) << "Version " << '|' << std::setw(18) << "GM-commands " << '|' << '\n';
		std::cout << '+' << std::setfill('-') << std::setw(7) << '+' << std::setw(49) << '+' << std::setw(18) << '+' << std::setw(19) << '+' << '\n';

		unsigned int n = 0;
		
		for (const auto& p : m_database) {
			std::cout << '|' << std::setfill(' ') << std::setw(5) << ++n   << '.'
				      << '|' << std::setw(47) << p.second.name             << ' '
				      << '|' << std::setw(16) << p.first                   << ' '
				      << '|' << std::setw(17) << p.second.addresses.size() << ' '
				      << '|' << '\n';
		}

		std::cout << '+' << std::setfill('-') << std::setw(7) << '+' << std::setw(49) << '+' << std::setw(18) << '+' << std::setw(19) << '+' << '\n';

		return ErrorCode::Ok;
	}

}