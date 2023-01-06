#include <iostream>

#include "Path.h"
#include "CheckExecutableCommand.h"


namespace pkodev {

	std::string CCheckExecutableCommand::GetName() const
	{
		return std::string{ "check" };
	}

	std::string CCheckExecutableCommand::GetDescription() const
	{
		return std::string{ "Check if an .exe file is supported by the tool." };
	}

	std::string CCheckExecutableCommand::Usage() const
	{
		return std::string{ GetName() + CFileRequiredCommand::Usage() + CAddressRequiredCommand::Usage() };
	}

	ICommand::RequiredArgs CCheckExecutableCommand::GetRequiredArgs() const
	{
		RequiredArgs args;
		utils::output::Merge(args, CAddressRequiredCommand::GetRequiredArgs());
		utils::output::Merge(args, CFileRequiredCommand::GetRequiredArgs());

		return args;
	}

	ICommand::ErrorCode CCheckExecutableCommand::Execute(const Args& args)
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

		if (it != m_database.cend()) 
		{
			const unsigned int& version = it->first;
			const std::string& name = it->second.name;
			
			std::cout << name << " (Version: " << version << ") is supported!" << std::endl;
		}
		else 
		{
			const std::string& name = utils::Path::ExtractFileName(args.at("in"));
			std::cout << "The executable file '" << name 
				<< "' is NOT supported!" << std::endl;
		}
		
		return ErrorCode::Ok;
	}

}