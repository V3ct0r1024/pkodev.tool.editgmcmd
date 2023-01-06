#pragma once
#include "Command.h"


namespace pkodev {

	class CBackupRequiredCommand final : public ICommand
	{
		public:

			CBackupRequiredCommand(ICommand::Ptr command) :
				m_command(command) { };
			~CBackupRequiredCommand() = default;

			std::string GetName() const override { return m_command->GetName(); }
			std::string GetDescription() const override { return m_command->GetDescription(); }
			std::string Usage() const override;
			RequiredArgs GetRequiredArgs() const override;
			ErrorCode Execute(const Args& args) override;

		private:

			enum class BackupMode : unsigned int {
				None = 0,
				JustName,
				NameAndTime,
				Undefined,
			};

			static BackupMode GetBackupMode(const std::string& mode);

			ICommand::Ptr m_command;
	};

}

