#pragma once
#include "AddressRequiredCommand.h"


namespace pkodev {

	class CSupportedCommand final : public CAddressRequiredCommand
	{
		public:
			CSupportedCommand() = default;
			~CSupportedCommand() = default;
			std::string GetName() const override;
			std::string GetDescription() const override;
			std::string Usage() const override;
			RequiredArgs GetRequiredArgs() const override;
			ErrorCode Execute(const Args& args) override;
	};

	class CSupportedCommandFactory final : public ICommandFactory
	{
		public:
			ICommand::Ptr Create() override {
				return std::make_shared<CSupportedCommand>();
			}
	};

}
