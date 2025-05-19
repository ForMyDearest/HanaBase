#include <doctest/doctest.h>

#define HANA_LOG_ENABLE
#include <hana/log.hpp>
#include <hana/hash.hpp>
#include <hana/guid.hpp>

TEST_CASE("xxhash") {
	using namespace hana;

	static constexpr const char* str = "123456";
	constexpr auto val = Hash<const char*>()(str);
	HStringView sv = str;
	CHECK_EQ(Hash<HStringView>()(sv), val);
}

TEST_CASE("type traits") {
	using namespace hana;

	CHECK_FALSE(is_iterator_v<void>);
	CHECK_FALSE(is_iterator_v<int>);

	CHECK_FALSE(is_iterator_v<void *>);
	CHECK(is_iterator_v<int *>);

	CHECK(is_iterator_v<std::vector<int>::iterator>);
	CHECK(is_iterator_v<std::vector<int>::const_iterator>);
	CHECK(is_iterator_v<std::vector<int>::reverse_iterator>);

	CHECK(is_specialization_v<std::vector<int>, std::vector>);
}

TEST_CASE("guid") {
	using namespace hana;
	constexpr auto g1 = guid_t::from_string(u8"1d29de69-b2b0-44f6-a1aa-4acf070bf8bb");
	constexpr auto g2 = guid_t::from_string(u8"{1d29de69-b2b0-44f6-a1aa-4acf070bf8bb}");
	CHECK_EQ(g1.value(), g2.value());
	CHECK_EQ(g2->to_string(), u8"1d29de69-b2b0-44f6-a1aa-4acf070bf8bb");
	LOG_INFO(u8"{}", guid_t::create()->to_string());
}
