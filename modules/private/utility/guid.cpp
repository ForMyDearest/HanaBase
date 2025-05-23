#include <hana/utility/guid.hpp>

#if defined(_WIN32)
#	include "guid/win.cpp"
#elif defined(__linux__) || defined(__unix__)
#	include "guid/unix.cpp"
#elif defined(__APPLE__)
#	include "guid/apple.cpp"
#else
#	include "guid/standard.cpp"
#endif

namespace hana
{
	HString guid_t::to_string() const noexcept {
		static constexpr char8_t guid_encoder[17] = u8"0123456789abcdef";
		static constexpr char8_t empty_guid[37] = u8"00000000-0000-0000-0000-000000000000";

		HString uustr{empty_guid};

		for (size_t i = 0, index = 0; i < 36; ++i) {
			if (i == 8 || i == 13 || i == 18 || i == 23) {
				continue;
			}
			uustr[i] = guid_encoder[data[index] >> 4 & 0x0f];
			uustr[++i] = guid_encoder[data[index] & 0x0f];
			index++;
		}

		return uustr;
	}
}
