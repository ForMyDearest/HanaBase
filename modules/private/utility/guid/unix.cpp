#include <uuid/uuid.h>

namespace hana
{
	std::optional<guid_t> guid_t::create() noexcept {
		uuid_t id;
		uuid_generate(id);

		std::array<uint8_t, 16> bytes =
		{{
			id[0],
			id[1],
			id[2],
			id[3],
			id[4],
			id[5],
			id[6],
			id[7],
			id[8],
			id[9],
			id[10],
			id[11],
			id[12],
			id[13],
			id[14],
			id[15]
		}};

		return guid_t{std::begin(bytes), std::end(bytes)};
	}
}
