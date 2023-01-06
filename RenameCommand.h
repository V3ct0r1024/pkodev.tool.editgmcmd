#pragma once
#include "FileRequiredCommand.h"
#include "AddressRequiredCommand.h"
#include "BackupRequiredCommand.h"


namespace pkodev {

	class CRenameCommand final : public CFileRequiredCommand,
								 public CAddressRequiredCommand
	{
		public:
			CRenameCommand() = default;
			~CRenameCommand() = default;
			std::string GetName() const override;
			std::string GetDescription() const override;
			std::string Usage() const override;
			RequiredArgs GetRequiredArgs() const override;
			ErrorCode Execute(const Args& args) override;
	};

	class CRenameCommandFactory final : public ICommandFactory
	{
		public:
			ICommand::Ptr Create() override {
				return std::make_shared<CBackupRequiredCommand>(
					std::make_shared<CRenameCommand>()
				);
			}
	};

}
