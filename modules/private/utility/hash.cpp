#include <hana/utility/hash.hpp>

#include <xxh3.h>

namespace hana
{
	uint32_t XXHash::_xxhash32(const void* input, size_t length, uint32_t seed) noexcept {
		return XXH32(input, length, seed);
	}

	uint64_t XXHash::_xxhash64(const void* input, size_t length) noexcept {
		return XXH3_64bits(input, length);
	}

	uint64_t XXHash::_xxhash64(const void* input, size_t length, uint64_t seed) noexcept {
		return XXH3_64bits_withSeed(input, length, seed);
	}
}
