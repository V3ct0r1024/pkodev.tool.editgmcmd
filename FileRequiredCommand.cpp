#include <Windows.h>

#include "Path.h"
#include "FileRequiredCommand.h"


namespace pkodev {

	CFileRequiredCommand::CFileRequiredCommand() :
		m_build_timestamp(0)
	{

	}

	std::string CFileRequiredCommand::Usage() const
	{
		return std::string{ " --in <.exe file>" };
	}

	ICommand::RequiredArgs CFileRequiredCommand::GetRequiredArgs() const
	{
		return { { "in", Argument::Required::Mandatory } };
	}

	ICommand::ErrorCode CFileRequiredCommand::Execute(const Args& args)
	{
		const std::string& in = args.at("in");

		m_file.open(in, std::ios::binary | std::ios::in | std::ios::out);

		if (m_file.is_open() == false) {
			std::cout << "Can't open the executable file '" << utils::Path::ExtractFileName(in) << "'!" << std::endl;
			return ErrorCode::Error;
		}

		m_file.exceptions(std::fstream::badbit | std::fstream::failbit | std::fstream::eofbit);

		char header[1024]{0x00};
		m_file.read(header, sizeof(header));

		PIMAGE_DOS_HEADER pidh = reinterpret_cast<PIMAGE_DOS_HEADER>(header);
		PIMAGE_NT_HEADERS pinh = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<BYTE*>(pidh) + pidh->e_lfanew);
		PIMAGE_FILE_HEADER pifh = reinterpret_cast<PIMAGE_FILE_HEADER>(&pinh->FileHeader);

		m_build_timestamp = static_cast<unsigned int>(pifh->TimeDateStamp);

		return ErrorCode::Ok;
	}

	unsigned int CFileRequiredCommand::SearchBlockOfBytes(unsigned int size)
	{
		try
		{
			size += 1 + 8; // 1 byte left offset from probably previous null-terminated string
			// 8 bytes right offset for probably numeric values

			m_file.seekg(0x00100000);
			
			unsigned int counter = 0;
			char ch{ 0x00 };

			while (m_file.get(ch))
			{
				if (ch == 0x00) {
					++counter;
				}
				else {
					counter = 0;
					continue;
				}

				if (counter == size) 
				{
					return static_cast<unsigned int>(m_file.tellg()) - counter + 1;
				}
			}
		}
		catch (const std::fstream::failure&) {
			if (m_file.eof() == false) {
				const unsigned int address = static_cast<unsigned int>(m_file.tellg());
				std::cout << "Error while searching block of " << size << " null bytes at address " 
					<< utils::output::address(address) << '.' << std::endl;
				throw;
			}
		}

		return 0x00000000;
	}

	bool CFileRequiredCommand::CheckBlockOfBytes(unsigned int address, const std::string& id,
		char flag, unsigned short int& ref_size, unsigned int& ref_addr)
	{
		ref_size = 0;
		ref_addr = 0x00000000;

		try
		{
			m_file.seekp(address);

			char ch{ 0x00 };
			unsigned int counter = 0;

			while (m_file.get(ch))
			{
				if (ch == 0x00) {
					break;
				}

				if (ch != id[counter++]) {
					break;
				}
			}

			if ( (counter + 1) == (id.length() + 1) ) {

				ref_size = Read<unsigned short int>();
				ref_addr = Read<unsigned int>();
				ch = Read<char>(ch);

				if (static_cast<unsigned char>(ch) == static_cast<unsigned char>(flag)) {
					return true;
				}
			}
		}
		catch (const std::fstream::failure&) {
			if (m_file.eof() == false) {
				std::cout << "Error while checking the extended block of bytes at address "
					<< utils::output::address(address) << '.' << std::endl;

				throw;
			}
		}

		return false;
	}

	void CFileRequiredCommand::ZeroBytes(unsigned int n, unsigned int address)
	{
		try
		{
			m_file.seekp(address);

			for (unsigned int i = 0; i < n; ++i) {
				m_file.put(0x00);
			}
		}
		catch (const std::fstream::failure&) {
			std::cout << "Failed to zero " << n << " bytes at adrress " 
				<< utils::output::address(address) << '.' << std::endl;
			throw;
		}
	}

}
