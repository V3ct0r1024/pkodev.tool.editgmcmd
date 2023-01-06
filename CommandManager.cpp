#include <algorithm>
#include "CommandManager.h"


namespace pkodev {

	void CCommandManager::AddCommand(const std::string& name, ICommandFactory::Ptr creator)
	{
		m_creators.emplace(name, creator);
	}

	void CCommandManager::RemoveCommand(const std::string& name)
	{
		m_creators.erase(name);
	}

	bool CCommandManager::CheckCommand(const std::string& name) const
	{
		return (m_creators.find(name) != m_creators.cend());
	}

	ICommand::Ptr CCommandManager::GetCommand(const std::string& name) const
	{
		auto it = m_creators.find(name);

		if (it == m_creators.end()) {
			throw command_mgr_exception("Unknown command name '" + name + "'!");
		}

		return it->second->Create();
	}

	const std::vector<std::string> CCommandManager::EnumCommands() const
	{
		std::vector<std::string> commands;

		for (const auto& creator : m_creators) {
			commands.push_back(creator.first);
		}

		return commands;
	}

 }