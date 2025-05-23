#include <CoreFoundation/CFUUID.h>

namespace hana
{
	std::optional<guid_t> guid_t::create() noexcept {
		auto newId = CFUUIDCreate(NULL);
		auto bytes = CFUUIDGetUUIDBytes(newId);
		CFRelease(newId);

		std::array<uint8_t, 16> arrbytes =
		{{
			bytes.byte0,
			bytes.byte1,
			bytes.byte2,
			bytes.byte3,
			bytes.byte4,
			bytes.byte5,
			bytes.byte6,
			bytes.byte7,
			bytes.byte8,
			bytes.byte9,
			bytes.byte10,
			bytes.byte11,
			bytes.byte12,
			bytes.byte13,
			bytes.byte14,
			bytes.byte15
		}};
		return guid_t{std::begin(arrbytes), std::end(arrbytes)};
	}
}
