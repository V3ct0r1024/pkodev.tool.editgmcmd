#pragma once
#include <unordered_map>
#include "Command.h"


namespace pkodev {

	class CAddressRequiredCommand : public virtual ICommand
	{
		public:

			CAddressRequiredCommand() = default;
			virtual ~CAddressRequiredCommand() = default;
			virtual std::string Usage() const override;
			virtual	RequiredArgs GetRequiredArgs() const override;
			virtual ErrorCode Execute(const Args& args) override;

		private:

			static bool ValidateName(const std::string& name);
			static bool ValidateAddress(const std::string& address);

		protected:

			Database m_database;
			
	};

}
