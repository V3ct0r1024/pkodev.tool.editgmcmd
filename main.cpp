#include <iostream>
#include <string>
#include <memory>
#include <deque>
#include <algorithm>

#include "Output.h"
#include "Path.h"
#include "String.h"

#include "CommandManager.h"
#include "NameCommand.h"
#include "RenameCommand.h"
#include "EnumCommand.h"
#include "SupportedCommand.h"
#include "CheckExecutableCommand.h"
#include "ResetCommand.h"



int main(int argc, char **argv)
{
    std::cout << "PKOdev.NET GM-commands editor tool" << std::endl;
    std::cout << "Author: V3ct0r from forum PKOdev.NET" << std::endl;
    std::cout << "Version: 1.0 (11/07/2022)" << std::endl << std::endl;

    std::unique_ptr<pkodev::ICommandManager> manager = std::make_unique<pkodev::CCommandManager>();
    manager->AddCommand("name", std::make_shared<pkodev::CNameCommandFactory>());
    manager->AddCommand("rename", std::make_shared<pkodev::CRenameCommandFactory>());
    manager->AddCommand("enum", std::make_shared<pkodev::CEnumCommandFactory>());
    manager->AddCommand("supported", std::make_shared<pkodev::CSupportedCommandFactory>());
    manager->AddCommand("check", std::make_shared<pkodev::CCheckExecutableCommandFactory>());
    manager->AddCommand("reset", std::make_shared<pkodev::CResetCommandFactory>());

    std::deque<std::string> params;
    for (int i = 1; i < argc; ++i) {
        params.emplace_back(std::string{ argv[i] });
    }

    if ( (params.empty() == true) || (params.front() == "help") || (manager->CheckCommand(params.front()) == false) )
    {
        const std::string path = pkodev::utils::Path::ExtractFileName(argv[0]);

        std::cout << "Usage: " << path << " <command> [parameters]" << std::endl << std::endl;
        std::cout << "There are supported commands:" << std::endl;

        unsigned int n = 0;

        for (const auto& name : manager->EnumCommands()) 
        {
            const auto command = manager->GetCommand(name);

            std::cout << ++n << ") " << command->Usage() << std::endl;
            std::cout << '\t' << command->GetDescription() << std::endl;
        }
        
        std::cout << std::endl << "Parameters in the curly braces {} are optional." << std::endl;
        std::cout << "'" << path << " help' list available commands." << std::endl;

        if ( (params.empty() == false) && (params.front() == "help") ) {
            return static_cast<int>(pkodev::ICommand::ErrorCode::Ok);
        } 
        else {
            return static_cast<int>(pkodev::ICommand::ErrorCode::WrongArgs);
        }
    }

    const std::string name{ params.front() };
    params.pop_front();

    try
    {
        pkodev::ICommand::Args args;

        for (auto it = params.cbegin(); it != params.cend(); ++it) {

            auto is_key = [](const std::string& value) -> bool
            {
                return (value.length() > 2 && value[0] == '-' && value[1] == '-');
            };

            if ((*it).empty() == true) {
                continue;
            }

            if (is_key(*it) == true)
            {
                auto next = it + 1;

                if ( (next != params.end()) && (is_key(*next) == false) ) {

                    args.insert({ (*it).substr(2), *next });
                    ++it;

                }
            }

        }

        auto command = manager->GetCommand(name);

        std::vector<std::string> to_check;
        for (const auto& a : args) {
            to_check.push_back(a.first);
        }

        bool check_args = true;
        for (const auto& argument : command->GetRequiredArgs()) {
            
            const auto it = std::find_if(to_check.begin(), to_check.end(),
                [&argument](const auto& name) -> bool { return (argument.name == name); });

            if (argument.required == pkodev::ICommand::Argument::Required::Mandatory) {

                if (it == to_check.end()) {
                    std::cout << "Mandatory parameter '--" << argument.name << "' is missing!" << std::endl;
                    check_args = false;
                    break;
                }

            }

            if (it != to_check.end()) {
                to_check.erase(it);
            }

        }

        if ( (check_args == true) && (to_check.empty() == false) ) {
            std::cout << "Extra parameter '--" << to_check.front() << "' is given!" << std::endl;
            check_args = false;
        }

        pkodev::ICommand::ErrorCode ret = pkodev::ICommand::ErrorCode::Ok;

        if (check_args == false) {
            ret = pkodev::ICommand::ErrorCode::WrongArgs;
        }
        else {
            ret = command->Execute(args);
        }

        std::cout << "(" << static_cast<unsigned int>(ret) << ") ";

        switch (ret)
        {
            case pkodev::ICommand::ErrorCode::Ok:
                std::cout << "The command '" << name << "' executed successfully." << std::endl;
                break;

            case pkodev::ICommand::ErrorCode::WrongArgs:
                std::cout << "Incorrect parameters are given." << std::endl
                    << "Usage: " << command->Usage() << std::endl;
                break;

            case pkodev::ICommand::ErrorCode::Unsupported:
                {
                    const auto it = args.find("in");
                    if (it != args.end()) {
                        std::cout << "The executable file '"
                            << pkodev::utils::Path::ExtractFileName(it->second)
                            << "' is not supported." << std::endl;
                    }
                    else {
                        std::cout << "The executable file is not supported." << std::endl;
                    }
                }
                break;

            case pkodev::ICommand::ErrorCode::Error:
                std::cout << "An error occurred while executing the '"
                    << name << "' command." << std::endl;
                break;
        }
        
        return static_cast<int>(ret);
    }
    catch (const pkodev::command_mgr_exception& e) {
        std::cout << "Command manager error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Critical error: " << e.what() << std::endl;
    }
    catch (...)  {
        std::cout << "Unknown error!" << std::endl;
    }

    return static_cast<int>(pkodev::ICommand::ErrorCode::Error);
}