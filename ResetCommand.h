#pragma once
#include "FileRequiredCommand.h"
#include "AddressRequiredCommand.h"
#include "BackupRequiredCommand.h"


namespace pkodev {

	class CResetCommand final : public CFileRequiredCommand, 
							   public CAddressRequiredCommand
	{
		public:
			CResetCommand() = default;
			~CResetCommand() = default;
			std::string GetName() const override;
			std::string GetDescription() const override;
			std::string Usage() const override;
			RequiredArgs GetRequiredArgs() const override;
			ErrorCode Execute(const Args& args) override;
	};

	class CResetCommandFactory final : public ICommandFactory
	{
		public:
			ICommand::Ptr Create() override {
				return std::make_shared<CBackupRequiredCommand>(
					std::make_shared<CResetCommand>()
				);
			}
	};

}
