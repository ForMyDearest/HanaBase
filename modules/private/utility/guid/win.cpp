#include <hana/dll/shared_library.hpp>

#include <objbase.h>

namespace hana
{
	std::optional<guid_t> guid_t::create() noexcept {
		GUID newId;
		SharedLibrary lib;
		lib.load(u8"Ole32.dll", SharedLibrary::search_system_folders);
		auto pfnCoCreateGuid = lib.get<decltype(CoCreateGuid)>(u8"CoCreateGuid");
		const HRESULT hr = pfnCoCreateGuid(&newId);
		if (FAILED(hr)) {
			return {};
		}

		std::array<uint8_t, 16> bytes =
		{{
			static_cast<unsigned char>((newId.Data1 >> 24) & 0xFF),
			static_cast<unsigned char>((newId.Data1 >> 16) & 0xFF),
			static_cast<unsigned char>((newId.Data1 >> 8) & 0xFF),
			static_cast<unsigned char>((newId.Data1) & 0xFF),

			static_cast<unsigned char>((newId.Data2 >> 8) & 0xFF),
			static_cast<unsigned char>((newId.Data2) & 0xFF),

			static_cast<unsigned char>((newId.Data3 >> 8) & 0xFF),
			static_cast<unsigned char>((newId.Data3) & 0xFF),

			newId.Data4[0],
			newId.Data4[1],
			newId.Data4[2],
			newId.Data4[3],
			newId.Data4[4],
			newId.Data4[5],
			newId.Data4[6],
			newId.Data4[7]
		}};

		return guid_t{std::begin(bytes), std::end(bytes)};
	}
}
