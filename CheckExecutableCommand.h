#pragma once
#include "FileRequiredCommand.h"
#include "AddressRequiredCommand.h"


namespace pkodev {

	class CCheckExecutableCommand final : public CFileRequiredCommand,
										  public CAddressRequiredCommand
	{
		public:
			CCheckExecutableCommand() = default;
			~CCheckExecutableCommand() = default;
			std::string GetName() const override;
			std::string GetDescription() const override;
			std::string Usage() const override;
			RequiredArgs GetRequiredArgs() const override;
			ErrorCode Execute(const Args& args) override;
	};

	class CCheckExecutableCommandFactory final : public ICommandFactory
	{
		public:
			ICommand::Ptr Create() override {
				return std::make_shared<CCheckExecutableCommand>();
			}
	};

}
