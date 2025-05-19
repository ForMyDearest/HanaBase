#pragma once

#include "string.hpp"

#include <span>
#include <optional>

namespace hana
{
	class guid_t {
	public:
		// indicated by a bit pattern in octet 8, marked with N in xxxxxxxx-xxxx-xxxx-Nxxx-xxxxxxxxxxxx
		enum class Variant {
			// NCS backward compatibility (with the obsolete Apollo Network Computing System 1.5 GUID format)
			// N bit pattern: 0xxx
			// > the first 6 octets of the GUID are a 48-bit timestamp (the number of 4 microsecond units of time since 1 Jan 1980 UTC);
			// > the next 2 octets are reserved;
			// > the next octet is the "address family";
			// > the final 7 octets are a 56-bit host ID in the form specified by the address family
			ncs,

			// RFC 4122/DCE 1.1
			// N bit pattern: 10xx
			// > big-endian byte order
			rfc,

			// Microsoft Corporation backward compatibility
			// N bit pattern: 110x
			// > little endian byte order
			// > formely used in the Component Object Model (COM) library
			microsoft,

			// reserved for possible future definition
			// N bit pattern: 111x
			reserved
		};

		// indicated by a bit pattern in octet 6, marked with M in xxxxxxxx-xxxx-Mxxx-xxxx-xxxxxxxxxxxx
		enum class Version {
			none                = 0, // only possible for nil or invalid guids
			time_based          = 1, // The time-based version specified in RFC 4122
			dce_security        = 2, // DCE Security version, with embedded POSIX UIDs.
			name_based_md5      = 3, // The name-based version specified in RFS 4122 with MD5 hashing
			random_number_based = 4, // The randomly or pseudo-randomly generated version specified in RFS 4122
			name_based_sha1     = 5  // The name-based version specified in RFS 4122 with SHA1 hashing
		};

		using value_type = uint8_t;

		HANA_BASE_API static std::optional<guid_t> create() noexcept;

		static constexpr std::optional<guid_t> from_string(HStringView str) noexcept {
			bool firstDigit = true;
			size_t hasBraces = 0;
			size_t index = 0;

			std::array<uint8_t, 16> data{{0}};

			if (str.empty()) return {};

			if (str.front() == '{') hasBraces = 1;
			if (hasBraces && str.back() != '}') return {};

			for (size_t i = hasBraces; i < str.size() - hasBraces; ++i) {
				if (str[i] == '-') continue;

				if (index >= 16) return {};

				char8_t ch;
				if (str[i] >= '0' && str[i] <= '9') ch = str[i] - '0';
				else if (str[i] >= 'a' && str[i] <= 'f') ch = str[i] - 'a' + 10;
				else if (str[i] >= 'A' && str[i] <= 'F') ch = str[i] - 'A' + 10;
				else return {};

				if (firstDigit) {
					data[index] = static_cast<uint8_t>(ch << 4);
					firstDigit = false;
				} else {
					data[index] = static_cast<uint8_t>(data[index] | ch);
					index++;
					firstDigit = true;
				}
			}

			if (index < 16) {
				return {};
			}

			return guid_t{data};
		}

		[[nodiscard]] HANA_BASE_API HString to_string() const noexcept;

		constexpr guid_t() noexcept = default;

		constexpr explicit guid_t(value_type (&arr)[16]) noexcept {
			std::ranges::copy(arr, std::begin(data));
		}

		constexpr explicit guid_t(const std::array<value_type, 16>& arr) noexcept : data{arr} {}

		constexpr explicit guid_t(std::span<value_type, 16> bytes) {
			std::ranges::copy(bytes, std::begin(data));
		}

		template<typename ForwardIterator>
		explicit guid_t(ForwardIterator first, ForwardIterator last) {
			if (std::distance(first, last) == 16) {
				std::copy(first, last, std::begin(data));
			}
		}

		[[nodiscard]] constexpr Variant variant() const noexcept {
			using enum Variant;
			if ((data[8] & 0x80) == 0x00) return ncs;
			if ((data[8] & 0xC0) == 0x80) return rfc;
			if ((data[8] & 0xE0) == 0xC0) return microsoft;
			return reserved;
		}

		[[nodiscard]] constexpr Version version() const noexcept {
			using enum Version;
			if ((data[6] & 0xF0) == 0x10) return time_based;
			if ((data[6] & 0xF0) == 0x20) return dce_security;
			if ((data[6] & 0xF0) == 0x30) return name_based_md5;
			if ((data[6] & 0xF0) == 0x40) return random_number_based;
			if ((data[6] & 0xF0) == 0x50) return name_based_sha1;
			return none;
		}

		[[nodiscard]] constexpr bool is_nil() const noexcept {
			for (const unsigned char i: data) if (i != 0) return false;
			return true;
		}

		constexpr void swap(guid_t& other) noexcept {
			data.swap(other.data);
		}

		[[nodiscard]] constexpr std::span<const std::byte, 16> as_bytes() const {
			return std::span<const std::byte, 16>(reinterpret_cast<const std::byte*>(data.data()), 16);
		}

		[[nodiscard]] constexpr static bool is_valid_guid(std::basic_string_view<char8_t> str) noexcept {
			auto is_hex = [](char8_t ch) {
				return
						(ch >= '0' && ch <= '9') ||
						(ch >= 'a' && ch <= 'f') ||
						(ch >= 'A' && ch <= 'F');
			};

			bool firstDigit = true;
			size_t hasBraces = 0;
			size_t index = 0;

			if (str.empty())
				return false;

			if (str.front() == '{')
				hasBraces = 1;
			if (hasBraces && str.back() != '}')
				return false;

			for (size_t i = hasBraces; i < str.size() - hasBraces; ++i) {
				if (str[i] == '-') continue;

				if (index >= 16 || !is_hex(str[i])) {
					return false;
				}

				if (firstDigit) {
					firstDigit = false;
				} else {
					index++;
					firstDigit = true;
				}
			}

			if (index < 16) {
				return false;
			}

			return true;
		}

		constexpr bool operator==(const guid_t& rhs) const noexcept {
			return data == rhs.data;
		}

		constexpr std::strong_ordering operator<=>(const guid_t& rhs) const noexcept {
			return data <=> rhs.data;
		}

	private:
		std::array<value_type, 16> data{{0}};
	};
}
