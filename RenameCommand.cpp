#include <iostream>
#include <iomanip>
#include <algorithm>

#include "Path.h"
#include "Output.h"
#include "RenameCommand.h"


namespace pkodev {

	std::string CRenameCommand::GetName() const
	{
		return std::string{ "rename" };
	}

	std::string CRenameCommand::GetDescription() const
	{
		return std::string{ "Rename a given GM-command." };
	}

	std::string CRenameCommand::Usage() const
	{
		return std::string{ GetName() + CFileRequiredCommand::Usage() + 
			" --old <GM-command> --new <GM-command>" + CAddressRequiredCommand::Usage() };
	}

	ICommand::RequiredArgs CRenameCommand::GetRequiredArgs() const
	{
		RequiredArgs args;
		utils::output::Merge(args, CAddressRequiredCommand::GetRequiredArgs());
		utils::output::Merge(args, CFileRequiredCommand::GetRequiredArgs());

		args.push_back({ "old", Argument::Required::Mandatory });
		args.push_back({ "new", Argument::Required::Mandatory });
		
		return args;
	}

	ICommand::ErrorCode CRenameCommand::Execute(const Args& args)
	{
		std::string __old = args.at("old");
		const std::string& __new = args.at("new");

		if (__old.empty() == true) {
			std::cout << "Enter the old GM-command name!" << std::endl;
			return ErrorCode::Error;
		}

		if (__new.empty() == true) {
			std::cout << "Enter the new GM-command name!" << std::endl;
			return ErrorCode::Error;
		}

		if (__new.length() > 32) {
			std::cout << "The new GM-command name '" << __new << "' is too long!"
				" Use names up to 32 characters long." << std::endl;
			return ErrorCode::Error;
		}

		for (const auto& c : __new) {
			if (std::isalnum(c) == 0) {
				std::cout << "The new GM-command name '" << __new << "' contains" 
					" invalid symbols! Use [0-9a-Z] symbols for the name." << std::endl;
				return ErrorCode::Error;
			}
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

		std::transform(__old.begin(), __old.end(), __old.begin(), std::tolower);

		const Address& addresses = it->second.addresses;
		const auto address = addresses.find(__old);

		if (address == addresses.end()) {
			std::cout << "The old GM-command name '" << it->second.symbol + __old << "' is unknown!" << std::endl;
			return ErrorCode::Error;
		}

		for (const auto& p : addresses) 
		{
			const std::string name = Read<std::string>(Read<unsigned int>(p.second) - base_address);

			if (__new == name) {
				std::cout << "The new GM-command name '" << it->second.symbol + __new
					<< "' is already in use for the '" << it->second.symbol + p.first << "' GM-command!"
					<< std::endl;
				return ErrorCode::Error;
			}
		}

		const unsigned int cur_addr = Read<unsigned int>(address->second) - base_address;
		const std::string cur_name = Read<std::string>(cur_addr);

		const unsigned int cur_size = static_cast<unsigned int>(cur_name.length()) + 1;
		const unsigned int __new_size = static_cast<unsigned int>(__new.length()) + 1;
		const unsigned int __old_size = static_cast<unsigned int>(__old.length()) + 1;

		std::cout << "Current '" << address->first << " 'GM-command name: '" << it->second.symbol + cur_name << "'; "
			<< "Size: " << cur_size << " bytes; " 
			<< "Address: " << utils::output::address(cur_addr) << '.' << std::endl;

		unsigned int ref_addr = 0x00000000;
		unsigned short int ref_size = 0;

		const bool new_block = CheckBlockOfBytes(cur_addr + cur_size, 
			__old, flag, ref_size, ref_addr);

		if (new_block == true) {
			std::cout << utils::output::address(cur_addr) << " is extended block." << std::endl
				<< "Original address: " << utils::output::address(ref_addr) << '.' << std::endl
				<< "Original size: " << ref_size << " bytes. " << std::endl;
		} 

		const unsigned int old_block_size = cur_size + __old_size
			+ sizeof(ref_size) + sizeof(ref_addr) + sizeof(flag);

		if ( (new_block == true) && (__new_size == ref_size) )
		{
			Write<std::string>(__new, ref_addr);
			Write<unsigned int>(ref_addr + base_address, address->second);
			ZeroBytes(old_block_size, cur_addr);

			std::cout << "The new GM-command name '" << it->second.symbol + __new << "' has been "
				" written at the original address!" << std::endl;

			return ErrorCode::Ok;
		}

		if (__new_size == cur_size)
		{
			Write<std::string>(__new, cur_addr);

			std::cout << "The new GM-command name '" << it->second.symbol + __new << "' has been "
				" written at the same address!" << std::endl;

			return ErrorCode::Ok;
		}

		const unsigned int new_block_size = __new_size + __old_size
			+ sizeof(ref_size) + sizeof(ref_addr) + sizeof(flag);

		if (__new_size > cur_size) {

			const unsigned int block_addr = SearchBlockOfBytes(new_block_size);

			if (block_addr == 0x00000000) {
				std::cout << "Can't find  '" << new_block_size << "' bytes of a free"
					" space in the executable file to write the longer new GM-command"
					" name '" << it->second.symbol + __new << "'!" << std::endl;
				return ErrorCode::Error;
			}

			std::cout << "Found the new block of a free space at address "
				<< utils::output::address(block_addr) << '.' << std::endl;

			Write<std::string>(__new, block_addr);
			Write<std::string>(__old);
			Write<unsigned short int>((new_block == true) ? ref_size : cur_size);
			Write<unsigned int>((new_block == true) ? ref_addr : cur_addr);
			Write<char>(flag);

			Write<unsigned int>(block_addr + base_address, address->second);

			if (new_block == true) { ZeroBytes(old_block_size, cur_addr); }

			std::cout << "The new GM-command name '" << it->second.symbol + __new << "' has been "
				" written at the new address!" << std::endl;
		}
		else
		{
			if (new_block == true)
			{
				ZeroBytes(old_block_size, cur_addr);
				Write<std::string>(__new, cur_addr);
				Write<std::string>(__old);
				Write<unsigned short int>(ref_size);
				Write<unsigned int>(ref_addr);
				Write<char>(flag);

				std::cout << "The new GM-command name '" << it->second.symbol + __new << "' has been "
					" written at the same address!" << std::endl;
			}
			else
			{
				const unsigned int block_addr = SearchBlockOfBytes(new_block_size);

				if (block_addr == 0x00000000) {
					std::cout << "Can't find  '" << new_block_size << "' bytes of a free"
						" space in the executable file to write the shorter new GM-command"
						" name '" << it->second.symbol + __new << "'!" << std::endl;
					return ErrorCode::Error;
				}

				std::cout << "Found the new block of a free space at address "
					<< utils::output::address(block_addr) << '.' << std::endl;

				Write<std::string>(__new, block_addr);
				Write<std::string>(__old);
				Write<unsigned short int>(cur_size);
				Write<unsigned int>(cur_addr);
				Write<char>(flag);

				Write<unsigned int>(block_addr + base_address, address->second);

				std::cout << "The new GM-command name '" << it->second.symbol + __new << "' has been "
					" written at the new address!" << std::endl;
			}
		}

		return ErrorCode::Ok;
	}

}
