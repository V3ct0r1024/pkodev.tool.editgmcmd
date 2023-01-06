#pragma once
#include <iostream>
#include <iomanip>
#include <limits>


namespace pkodev { namespace utils { namespace output {

	class os_flag_saver final
	{
		public:
			explicit os_flag_saver(std::ostream& _ios) :
				ios(_ios),
				fmt_flags(_ios.flags()),
				fmt_state(nullptr)
			{
				fmt_state.copyfmt(_ios);
			}

			~os_flag_saver() { 
				ios.flags(fmt_flags);
				ios.copyfmt(fmt_state);
			}

			os_flag_saver(const os_flag_saver&) = delete;
			os_flag_saver& operator=(const os_flag_saver&) = delete;

		private:
			std::ostream& ios;
			std::ios fmt_state;
			std::ios_base::fmtflags fmt_flags;
	};
	
	template <typename T>
	void Merge(T& dst, T src)
	{
		dst.insert(dst.end(), std::make_move_iterator(src.begin()), std::make_move_iterator(src.end()));
	}

	std::string GetSystemDateTime();

	template<typename T>
	struct AddressProxy final {
		T value;
	};

	template<typename T>
	AddressProxy<T> address(const T& val) 
	{ 
		static_assert(std::numeric_limits<T>::is_integer, "Integer required.");
		return { val }; 
	}
	
	template<typename T>
	std::ostream& operator<<(std::ostream& os, const AddressProxy<T>& address) {
		os_flag_saver osfs(os);
		return os << "0x" << std::setw(8) << std::setfill('0')
			<< std::hex << std::uppercase << address.value << std::setw(0);
	}
	
}}}