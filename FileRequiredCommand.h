#pragma once
#include <fstream>
#include <iomanip>
#include <type_traits>

#include "Output.h"
#include "Command.h"

namespace pkodev {

	class CFileRequiredCommand : public virtual ICommand
	{
		public:

			CFileRequiredCommand();
			virtual ~CFileRequiredCommand() = default;
			virtual std::string Usage() const override;
			virtual	RequiredArgs GetRequiredArgs() const override;
			virtual ErrorCode Execute(const Args& args) override;

		protected:

			static const unsigned char flag = 0xBF;
			static const unsigned int base_address = 0x00400000;

			template<typename T>
			T Read(unsigned int address = 0x00000000);

			template<typename T>
			void Write(const T& value, unsigned int address = 0x00000000);

			unsigned int SearchBlockOfBytes(unsigned int size);
			bool CheckBlockOfBytes(unsigned int address, const std::string& id,
				char flag, unsigned short int& ref_size, unsigned int& ref_addr);
			void ZeroBytes(unsigned int n, unsigned int address);

			unsigned int m_build_timestamp;
			std::fstream m_file;
	};

	template<typename T>
	T CFileRequiredCommand::Read(unsigned int address)
	{
		static_assert(std::is_standard_layout<T>::value, "Data is too complex to read from a file.");

		T value{ 0 };

		try 
		{
			if (address != 0x00000000) {
				m_file.seekg(address);
			}
			else {
				address = static_cast<unsigned int>(m_file.tellg());
			}
			m_file.read(reinterpret_cast<char*>(&value), sizeof(value));
		}
		catch (const std::fstream::failure&) {
			std::cout << "Failed to read a value of a type (" << typeid(T).name() << ") at address " 
				<< utils::output::address(address) << '.' << std::endl;
			throw;
		}

		return value;
	}

	template<>
	inline std::string CFileRequiredCommand::Read<std::string>(unsigned int address)
	{
		std::string str{ "" };

		try 
		{
			if (address != 0x00000000) {
				m_file.seekg(address);
			}
			else {
				address = static_cast<unsigned int>(m_file.tellg());
			}

			char ch{ 0x00 };
			while (m_file.get(ch)) {
				if (ch == 0x00) {
					break;
				}
				str.push_back(ch);
			}
		}
		catch (const std::fstream::failure&) {
			std::cout << "Failed to read a value of a type (std::string) at address "
				<< utils::output::address(address) << '.' << std::endl;
			throw;
		}

		return str;
	}

	template<typename T>
	void CFileRequiredCommand::Write(const T& value, unsigned int address)
	{
		static_assert(std::is_standard_layout<T>::value, "Data is too complex to write to a file.");

		try
		{
			if (address != 0x00000000) {
				m_file.seekp(address);
			}
			else {
				address = static_cast<unsigned int>(m_file.tellp());
			}

			m_file.write(reinterpret_cast<const char*>(&value), sizeof(value));
		}
		catch (const std::fstream::failure&) {
			std::cout << "Failed to write a value of a type (" << typeid(T).name() << ") at address "
				<< utils::output::address(address) << '.' << std::endl;
			throw;
		}
	}

	template<>
	inline void CFileRequiredCommand::Write<std::string>(const std::string& value, unsigned int address)
	{
		try 
		{
			if (address != 0x00000000) {
				m_file.seekp(address);
			}
			else {
				address = static_cast<unsigned int>(m_file.tellp());
			}

			m_file.write(value.c_str(), value.length() + 1);
		}
		catch (const std::fstream::failure&) {
			std::cout << "Failed to write a value of a type (std::string) at address "
				<< utils::output::address(address) << '.' << std::endl;
			throw;
		}
	}

}
