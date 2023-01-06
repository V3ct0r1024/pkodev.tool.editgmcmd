#include <iostream>

#include "Output.h"
#include "ResetCommand.h"


namespace pkodev {

	std::string CResetCommand::GetName() const
	{
		return std::string{ "reset" };
	}

	std::string CResetCommand::GetDescription() const
	{
		return std::string{ "Reset all GM-command names to the original ones." };
	}

	std::string CResetCommand::Usage() const
	{
		return std::string{ GetName() + CFileRequiredCommand::Usage() + CAddressRequiredCommand::Usage() };
	}

	ICommand::RequiredArgs CResetCommand::GetRequiredArgs() const
	{
		RequiredArgs args;
		utils::output::Merge(args, CAddressRequiredCommand::GetRequiredArgs());
		utils::output::Merge(args, CFileRequiredCommand::GetRequiredArgs());

		return args;
	}

	ICommand::ErrorCode CResetCommand::Execute(const Args& args)
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

		for (const auto& p : it->second.addresses)
		{
			const unsigned int address = Read<unsigned int>(p.second) - base_address;
			const std::string value = Read<std::string>(address);

			if (value != p.first)
			{
				unsigned short int ref_size = 0;
				unsigned int ref_addr = 0x00000000;

				const unsigned int value_size = static_cast<unsigned int>(value.length()) + 1;
				
				const bool is_new_block = CheckBlockOfBytes(address + value_size,
					p.first, flag, ref_size, ref_addr);

				if (is_new_block == true) {
					
					const unsigned int id_size = static_cast<unsigned int>(p.first.length()) + 1;
					const unsigned int block_size = value_size + id_size
						+ sizeof(ref_size) + sizeof(ref_addr) + sizeof(flag);

					Write<std::string>(p.first, ref_addr);
					Write<unsigned int>(ref_addr + base_address, p.second);
					ZeroBytes(block_size, address);
					
				}
				else {
					Write<std::string>(p.first, address);
				}
			}
		}

		return ErrorCode::Ok;
	}

}