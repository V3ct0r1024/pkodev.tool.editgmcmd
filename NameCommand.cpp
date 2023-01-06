#include <iostream>
#include <algorithm>

#include "NameCommand.h"


namespace pkodev {

	std::string CNameCommand::GetName() const
	{
		return std::string{ "name" };
	}

	std::string CNameCommand::GetDescription() const
	{
		return std::string{ "Get the original name of a given GM-command." };
	}

	std::string CNameCommand::Usage() const
	{
		return std::string{ GetName() + CFileRequiredCommand::Usage() +
			" --old <GM-command>" + CAddressRequiredCommand::Usage() };
	}

	ICommand::RequiredArgs CNameCommand::GetRequiredArgs() const
	{
		RequiredArgs args;
		utils::output::Merge(args, CAddressRequiredCommand::GetRequiredArgs());
		utils::output::Merge(args, CFileRequiredCommand::GetRequiredArgs());

		args.push_back({ "old", Argument::Required::Mandatory });

		return args;
	}

	ICommand::ErrorCode CNameCommand::Execute(const Args& args)
	{
		std::string old = args.at("old");

		if (old.empty() == true) {
			std::cout << "Enter the old GM-command name!" << std::endl;
			return ErrorCode::Error;
		}

		ErrorCode ret = CAddressRequiredCommand::Execute(args);
		if (ret != ErrorCode::Ok) {
			return ret;
		}

		ret = CFileRequiredCommand::Execute(args);
		if (ret != ErrorCode::Ok) {
			return ret;
		}

		const auto it = m_database.find(m_build_timestamp);
		if (it == m_database.end()) {
			return ErrorCode::Unsupported;
		}

		std::transform(old.begin(), old.end(), old.begin(), std::tolower);

		const Address& addresses = it->second.addresses;
		const auto search = addresses.find(old);

		if (search == addresses.cend()) {
			std::cout << "GM-command '" << it->second.symbol + old << "' is unknown!" << std::endl;
			return ErrorCode::Ok;
		}

		const unsigned int cur_addr = Read<unsigned int>(search->second) - base_address;
		const std::string cur_name = Read<std::string>(cur_addr);

		std::cout << "The GM-command '" << it->second.symbol + old << "' is defined as '" << it->second.symbol + cur_name << "'." << std::endl;

		return ErrorCode::Ok;
	}

}