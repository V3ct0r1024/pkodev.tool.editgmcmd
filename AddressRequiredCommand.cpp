#include <iostream>
#include <fstream>
#include <string>

#include <nlohmann/json.hpp>

#include "Path.h"
#include "String.h"
#include "AddressRequiredCommand.h"


namespace pkodev {

	std::string CAddressRequiredCommand::Usage() const
	{
		return std::string{ " {--db <addresses file>}" };
	}

	ICommand::RequiredArgs CAddressRequiredCommand::GetRequiredArgs() const
	{
		return { { "db", Argument::Required::Optional } };
	}

	ICommand::ErrorCode CAddressRequiredCommand::Execute(const Args& args)
	{
		m_database.clear();

		std::string db_path{ "" };

		const auto it = args.find("db");

		if (it == args.cend()) {

			const std::string exe_name = utils::Path::GetExeName();
			if (exe_name.empty() == true) {
				std::cout << "Can't get the executable file name!" << std::endl;
				return ErrorCode::Error;
			}

			db_path = utils::Path::ChangeFileExt(exe_name, ".json");

		}
		else {

			db_path = it->second;

		}

		std::ifstream input(db_path);

		if (input.is_open() == false) {
			std::cout << "Can't open addresses database file '" << utils::Path::ExtractFileName(db_path) << "'!" << std::endl;
			return ErrorCode::Error;
		}

		using json = nlohmann::json;

		try
		{
			json in = json::parse(input);

			for (const auto& el : in.items()) 
			{
				const std::string& key = el.key();
				const json& value = el.value();

				std::pair<unsigned int, Version> tmp;
	
				tmp.first = std::stoul(utils::String::Trim(key));
				tmp.second.name = value.at("name");
				tmp.second.symbol = value.at("symbol");

				for (const auto& p : value.at("addresses").items()) 
				{
					std::string name = p.key();
					const std::string address = utils::String::Trim(p.value());

					std::transform(name.begin(), name.end(), name.begin(), std::tolower);

					if ( (ValidateName(name) == false) || (ValidateAddress(address) == false) )
					{
						std::cout << "Invalid <GM-command name>:<Address> pair: " 
							<< '"' << name << "\":\"" << address << "\"!" << std::endl;
						return ErrorCode::Error;
					}

					const unsigned int offset = std::stoul(address, nullptr, 16);

					const auto it = std::find_if(
						tmp.second.addresses.begin(), tmp.second.addresses.end(),
						[&offset](const auto& p) -> bool { return (offset == p.second); }
					);

					if (it != tmp.second.addresses.end()) {
						std::cout << "GM-commands '" << tmp.second.symbol + name << "' and '" << tmp.second.symbol + it->first
							<< "' are located at the same address 0x" << std::setfill('0')
							<< std::setw(8) << std::hex << std::uppercase << offset
							<< std::dec << std::nouppercase << ". Is this a mistake?" << std::endl;
						return ErrorCode::Error;
					}

					tmp.second.addresses.insert( { name,  offset } );
				}

				m_database.insert(tmp);
			}

			return ErrorCode::Ok;
		}
		catch (const json::exception& e) {
			std::cout << "JSON error while parsing addresses database: " << e.what() << std::endl;
		}
		catch (const std::exception& e) {
			std::cout << "Error while parsing addresses database: " << e.what() << std::endl;
		}
		
		return ErrorCode::Error;
	}

	bool CAddressRequiredCommand::ValidateName(const std::string& name)
	{
		return true;
	}

	bool CAddressRequiredCommand::ValidateAddress(const std::string& address)
	{
		if ( (address.length() != 10) || (address.find("0x") != 0) ) {
			return false;
		}

		for (auto it = (address.cbegin() + 2); it != address.cend(); ++it) {
			if (std::isxdigit(*it) == 0) {
				return false;
			}
		}

		return true;
	}

}
