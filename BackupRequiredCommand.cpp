#include <iostream>
#include <fstream>
#include <unordered_map>

#include "Path.h"
#include "Output.h"
#include "BackupRequiredCommand.h"

#include <Windows.h>


namespace pkodev {

	std::string CBackupRequiredCommand::Usage() const
	{
		return m_command->Usage().append(" {--bp <backup mode [off|name|time]>}");
	}

	ICommand::RequiredArgs CBackupRequiredCommand::GetRequiredArgs() const
	{
		auto args = m_command->GetRequiredArgs();

		args.push_back({ "bp", Argument::Required::Optional });

		return args;
	}

	ICommand::ErrorCode CBackupRequiredCommand::Execute(const Args& args)
	{

		BackupMode mode(BackupMode::JustName);

		if (args.find("bp") != args.end()) {

			const std::string& bp = args.at("bp");

			mode = GetBackupMode(bp);

			if (mode == BackupMode::Undefined) {
				std::cout << "Invalid backup mode '" << bp << "'!" << std::endl;
				return ErrorCode::Error;
			}

		}

		const std::string& from = args.at("in");
		std::string to{ "" };

		switch (mode)
		{
			case BackupMode::JustName:
				to = from + std::string{ ".backup" };
				break;

			case BackupMode::NameAndTime:
				to = utils::Path::ExtractFilePath(from) + '[' + utils::output::GetSystemDateTime() + ']' 
					+ utils::Path::ExtractFileName(from) + std::string{".backup"};
				break;

			case BackupMode::None:
				return m_command->Execute(args);
				break;
		}

		std::ifstream original(from, std::ios::binary);

		if (original.is_open() == false) {
			std::cout << "Can't open the executable file '" 
				<< utils::Path::ExtractFileName(from) << "' to make backup!" << std::endl;
			return ErrorCode::Error;
		}

		std::ofstream backup(to, std::ios::binary);

		if (backup.is_open() == false) {
			std::cout << "Can't create file '" << utils::Path::ExtractFileName(to)
				<< "' to make backup!" << std::endl;
			return ErrorCode::Error;
		}

		backup << original.rdbuf();

		original.close();
		backup.close();

		std::cout << "The file '" << utils::Path::ExtractFileName(from) 
			<< "' has been backed up to the file '" << utils::Path::ExtractFileName(to) << "'." << std::endl;

		return m_command->Execute(args);
	}

	CBackupRequiredCommand::BackupMode CBackupRequiredCommand::GetBackupMode(const std::string& mode)
	{
		std::unordered_map<std::string, BackupMode> modes = {
			{ "off", BackupMode::None},
			{ "name", BackupMode::JustName},
			{ "time", BackupMode::NameAndTime}
		};

		if (modes.find(mode) == modes.cend()) {
			return BackupMode::Undefined;
		}

		return modes.at(mode);
	}

}