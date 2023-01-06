#pragma once
#include <vector>
#include <unordered_map>
#include <stdexcept>

#include "Command.h"


namespace pkodev {

	class ICommandManager
	{
		public:
			virtual ~ICommandManager() = default;
			virtual void AddCommand(const std::string&, ICommandFactory::Ptr) = 0;
			virtual void RemoveCommand(const std::string&) = 0;
			virtual bool CheckCommand(const std::string&) const = 0;
			virtual ICommand::Ptr GetCommand(const std::string&) const = 0;
			virtual const std::vector<std::string> EnumCommands() const = 0;
	};

	class command_mgr_exception final : public std::runtime_error {
		public:
			command_mgr_exception(const std::string& what_arg) :
				std::runtime_error(what_arg) { }
			~command_mgr_exception() = default;
	};

	class CCommandManager final : public ICommandManager
	{
		public:

			CCommandManager() = default;
			~CCommandManager() = default;

			void AddCommand(const std::string& name, ICommandFactory::Ptr creator) override;
			void RemoveCommand(const std::string& name) override;
			bool CheckCommand(const std::string& name) const override;
			ICommand::Ptr GetCommand(const std::string& name) const override;
			const std::vector<std::string> EnumCommands() const override;

		private:
	
			std::unordered_map<std::string, ICommandFactory::Ptr> m_creators;
	};

}
