#pragma once
#include <string>
#include <unordered_map>
#include <memory>


namespace pkodev {

	class ICommand
	{
		public:

			enum class ErrorCode : unsigned int
			{
				Ok,
				Error,
				WrongArgs,
				Unsupported,
			};

			using Ptr = std::shared_ptr<ICommand>;
			using Args = std::unordered_map<std::string, std::string>;
			using Address = std::unordered_map<std::string, unsigned int>;
			
			struct Version { 
				std::string name; 
				std::string symbol;
				Address addresses; 
			};

			using Database = std::unordered_map<unsigned int, Version>;

			struct Argument {
				 
				enum class Required : unsigned int 
				{
					Optional = 0,
					Mandatory,
				};
				
				std::string name;
				Required required;
			
			};

			using RequiredArgs = std::vector<Argument>;

			virtual ~ICommand() = default;
			virtual std::string GetName() const = 0;
			virtual std::string GetDescription() const = 0;
			virtual std::string Usage() const = 0;
			virtual	RequiredArgs GetRequiredArgs() const = 0;
			virtual ErrorCode Execute(const Args&) = 0;
	};

	class ICommandFactory
	{
		public:

			using Ptr = std::shared_ptr<ICommandFactory>;

			virtual ~ICommandFactory() = default;
			virtual ICommand::Ptr Create() = 0;
	};

}