#pragma once
#include "FileRequiredCommand.h"
#include "AddressRequiredCommand.h"


namespace pkodev {

	class CNameCommand final : public CFileRequiredCommand, 
							   public CAddressRequiredCommand
	{
		public:
			CNameCommand() = default;
			~CNameCommand() = default;
			std::string GetName() const override;
			std::string GetDescription() const override;
			std::string Usage() const override;
			RequiredArgs GetRequiredArgs() const override;
			ErrorCode Execute(const Args& args) override;
	};

	class CNameCommandFactory final : public ICommandFactory
	{
		public:
			ICommand::Ptr Create() override {
				return std::make_shared<CNameCommand>();
			}
	};

}
