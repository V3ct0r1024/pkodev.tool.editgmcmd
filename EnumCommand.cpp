#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

#include "Output.h"
#include "EnumCommand.h"


namespace pkodev {

	std::string CEnumCommand::GetName() const
	{
		return std::string{ "enum" };
	}

	std::string CEnumCommand::GetDescription() const
	{
		return std::string{ "Print a list of GM-commands as a table." };
	}

	std::string CEnumCommand::Usage() const
	{
		return std::string{ GetName() + CFileRequiredCommand::Usage() + CAddressRequiredCommand::Usage() };
	}

	ICommand::RequiredArgs CEnumCommand::GetRequiredArgs() const
	{
		RequiredArgs args;
		utils::output::Merge(args, CAddressRequiredCommand::GetRequiredArgs());
		utils::output::Merge(args, CFileRequiredCommand::GetRequiredArgs());

		return args;
	}

	ICommand::ErrorCode CEnumCommand::Execute(const Args& args)
	{
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

		const Address& addresses = it->second.addresses;

		struct record {
			unsigned int n{0};
			std::string original_name;
			std::string current_name;
			unsigned int address{0x0000000};
			bool changed{false};
		};

		std::vector<record> records;

		for (const auto& p : addresses)
		{
			const unsigned int address = Read<unsigned int>(p.second) - base_address;

			record tmp;
			tmp.original_name = it->second.symbol + p.first;
			tmp.current_name = it->second.symbol + Read<std::string>(address);
			tmp.address = address;
			tmp.changed = (tmp.original_name != tmp.current_name);

			records.emplace_back(tmp);
		}

		std::sort(records.begin(), records.end(), [](const auto& p1, const auto& p2) -> bool
			{ return (p1.original_name < p2.original_name); });

		std::cout << '+' << std::setfill('-') << std::setw(7) << '+' << std::setw(33) << '+' << std::setw(33) << '+' << std::setw(17) << '+' << std::setw(17) << '+' << '\n';
		std::cout << '|' << std::setfill(' ') << std::setw(6) << "# " << '|' << std::setw(32) << "Original name " << '|' << std::setw(32) << "Curernt name " << '|' << std::setw(16) << "Address " << '|' << std::setw(16) << "Changed " << '|' << '\n';
		std::cout << '+' << std::setfill('-') << std::setw(7) << '+' << std::setw(33) << '+' << std::setw(33) << '+' << std::setw(17) << '+' << std::setw(17) << '+' << '\n';

		if (records.empty() == false) {
			unsigned int n = 0;
			for (const auto& r : records) {
				std::cout << '|' << std::setw(5)        << std::setfill(' ') << ++n               << '.'
					      << '|' << std::setw(31)       << r.original_name                        << ' '
					      << '|' << std::setw(31)       << r.current_name                         << ' ' 
					      << '|' << std::setw(5) << ' ' << utils::output::address(r.address)      << ' '
					      << '|' << std::setw(15)       << ( (r.changed == true) ? "Yes" : "No" ) << ' '
					      << '|' << '\n';
			}
		}
		else {
			std::cout << '|' << std::setfill(' ') << std::setw(106) << "There are no GM-commands " << '|' << '\n';
		}

		std::cout << '+' << std::setfill('-') << std::setw(7) << '+' << std::setw(33) << '+' << std::setw(33) << '+' << std::setw(17) << '+' << std::setw(17) << '+' << '\n';

		return ErrorCode::Ok;
	}

}