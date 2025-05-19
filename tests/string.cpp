#include <doctest/doctest.h>

#include <hana/string.hpp>

TEST_CASE("Test HString") {
	using namespace hana;
	using namespace hana::unicode;
	
	HStringView short_literal{u8"🐓鸡ĜG"};
	std::vector<char8_t> short_buffer_vec{short_literal.data(), short_literal.data() + short_literal.size() + 1};
	HStringView short_buffer{short_buffer_vec.data()};
	HStringView long_literal{u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀"};
	std::vector<char8_t> long_buffer_vec{long_literal.data(), long_literal.data() + long_literal.size() + 1};
	HStringView long_buffer{long_buffer_vec.data()};

	SUBCASE("ctor & dtor") {
		HString empty;
		CHECK_EQ(empty.size(), 0);
		CHECK_EQ(empty.capacity(), HString::SSOCapacity);

		auto test_ctors_of_view = [](const HStringView& view) {
			auto text_length = view.text_length();

			// ctor
			HString ctor{view.data()};
			CHECK_EQ(ctor.size(), view.size());

			// len ctor
			HString ctor_with_len{view.data(), view.size()};
			CHECK_EQ(ctor_with_len.size(), view.size());

			// view ctor
			HString view_ctor{view};
			CHECK_EQ(view_ctor.size(), view.size());
		};

		test_ctors_of_view(short_literal);
		test_ctors_of_view(short_buffer);
		test_ctors_of_view(long_literal);
		test_ctors_of_view(long_buffer);
	}

	SUBCASE("copy & move") {
		HString literal_a = short_literal;
		HString sso_a = short_buffer;
		HString heap_a = long_buffer;

		// copy literal
		HString literal_b = literal_a;
		CHECK_EQ(literal_b.size(), literal_a.size());
		CHECK_EQ(literal_b.capacity(), literal_a.capacity());
		CHECK_EQ(literal_b, literal_a);

		// copy sso
		HString sso_b = sso_a;
		CHECK_EQ(sso_b.size(), sso_a.size());
		CHECK_EQ(sso_b.capacity(), sso_a.capacity());
		CHECK_EQ(sso_b, sso_a);
		CHECK_NE(sso_b.data(), sso_a.data());

		// copy heap
		HString heap_b = heap_a;
		CHECK_EQ(heap_b.size(), heap_a.size());
		CHECK_EQ(heap_b.capacity(), heap_a.capacity());
		CHECK_EQ(heap_b, heap_a);
		CHECK_NE(heap_b.data(), heap_a.data());

		// move literal
		auto old_literal_a_size = literal_a.size();
		auto old_literal_a_capacity = literal_a.capacity();
		auto old_literal_a_data = literal_a.data();
		HString literal_c = std::move(literal_a);
		CHECK_EQ(literal_a.size(), 0);
		CHECK_EQ(literal_a.capacity(), HString::SSOCapacity);
		CHECK_EQ(literal_c.size(), old_literal_a_size);
		CHECK_EQ(literal_c.capacity(), old_literal_a_capacity);
		CHECK(literal_a.empty());

		// move sso
		auto old_sso_a_size = sso_a.size();
		auto old_sso_a_capacity = sso_a.capacity();
		HString sso_c = std::move(sso_a);
		CHECK_EQ(sso_a.size(), 0);
		CHECK_EQ(sso_a.capacity(), HString::SSOCapacity);
		CHECK_EQ(sso_c.size(), old_sso_a_size);
		CHECK_EQ(sso_c.capacity(), old_sso_a_capacity);
		CHECK(sso_a.empty());

		// move heap
		auto old_heap_a_size = heap_a.size();
		auto old_heap_a_capacity = heap_a.capacity();
		auto old_heap_a_data = heap_a.data();
		HString heap_c = std::move(heap_a);
		CHECK_EQ(heap_a.size(), 0);
		CHECK_GE(heap_a.capacity(), HString::SSOCapacity);
		CHECK_EQ(heap_c.size(), old_heap_a_size);
		CHECK_EQ(heap_c.capacity(), old_heap_a_capacity);
		CHECK_EQ(heap_c.data(), old_heap_a_data);
		CHECK(heap_a.empty());
	}

	SUBCASE("assign & move assign") {
		HString literal_a = short_literal;
		HString literal_b;
		HString literal_c;
		HString sso_a = short_buffer;
		HString sso_b;
		HString sso_c;
		HString heap_a = long_buffer;
		HString heap_b;
		HString heap_c;

		// assign literal
		literal_b = literal_a;
		CHECK_EQ(literal_b.size(), literal_a.size());
		CHECK_EQ(literal_b.capacity(), literal_a.capacity());
		CHECK_EQ(literal_b, literal_a);

		// assign sso
		sso_b = sso_a;
		CHECK_EQ(sso_b.size(), sso_a.size());
		CHECK_EQ(sso_b.capacity(), sso_a.capacity());
		CHECK_EQ(sso_b, sso_a);
		CHECK_NE(sso_b.data(), sso_a.data());

		// assign heap
		heap_b = heap_a;
		CHECK_EQ(heap_b.size(), heap_a.size());
		//CHECK_EQ(heap_b.capacity(), heap_a.capacity());
		CHECK_EQ(heap_b, heap_a);
		CHECK_NE(heap_b.data(), heap_a.data());

		// move assign literal
		auto old_literal_a_size = literal_a.size();
		auto old_literal_a_capacity = literal_a.capacity();
		auto old_literal_a_data = literal_a.data();
		literal_c = std::move(literal_a);
		CHECK_EQ(literal_a.size(), 0);
		CHECK_EQ(literal_a.capacity(), HString::SSOCapacity);
		CHECK_EQ(literal_c.size(), old_literal_a_size);
		CHECK_EQ(literal_c.capacity(), old_literal_a_capacity);
		CHECK(literal_a.empty());

		// move assign sso
		auto old_sso_a_size = sso_a.size();
		auto old_sso_a_capacity = sso_a.capacity();
		sso_c = std::move(sso_a);
		CHECK_EQ(sso_a.size(), 0);
		CHECK_EQ(sso_a.capacity(), HString::SSOCapacity);
		CHECK_EQ(sso_c.size(), old_sso_a_size);
		CHECK_EQ(sso_c.capacity(), old_sso_a_capacity);
		CHECK(sso_a.empty());

		// move assign heap
		auto old_heap_a_size = heap_a.size();
		auto old_heap_a_capacity = heap_a.capacity();
		auto old_heap_a_data = heap_a.data();
		heap_c = std::move(heap_a);
		CHECK_EQ(heap_a.size(), 0);
		CHECK_EQ(heap_a.capacity(), HString::SSOCapacity);
		CHECK_EQ(heap_c.size(), old_heap_a_size);
		CHECK_EQ(heap_c.capacity(), old_heap_a_capacity);
		CHECK_EQ(heap_c.data(), old_heap_a_data);
		CHECK(heap_a.empty());
	}

	SUBCASE("special assign") {
		auto test_assign_of_view = [](const HStringView& view) {
			auto text_length = view.text_length();

			// ctor
			HString ctor;
			ctor.assign(view.data());
			CHECK_EQ(ctor.size(), view.size());

			// len ctor
			HString ctor_with_len;
			ctor_with_len.assign(view.data(), view.size());
			CHECK_EQ(ctor_with_len.size(), view.size());

			// view ctor
			HString view_ctor;
			view_ctor.assign(view);
			CHECK_EQ(view_ctor.size(), view.size());
		};

		test_assign_of_view(short_literal);
		test_assign_of_view(short_buffer);
		test_assign_of_view(long_literal);
		test_assign_of_view(long_buffer);
	}

	SUBCASE("compare") {
		// test equal
		{
			HString a = short_literal;
			HString b = short_literal;
			HString c = short_buffer;
			HString d = long_literal;
			HString e = long_literal;
			HString f = long_buffer;

			CHECK_EQ(a, b);
			CHECK_EQ(a, c);
			CHECK_EQ(b, c);
			CHECK_EQ(d, e);
			CHECK_EQ(d, f);
			CHECK_EQ(e, f);
		}

		// test not equal
		{
			HString a = short_literal;
			HString b = short_literal.subview(short_literal.text_index_to_buffer(1));
			HString c = short_literal.subview(0, short_literal.text_index_to_buffer(short_buffer.text_length() - 1));
			HString d = long_literal;
			HString e = long_literal.subview(long_literal.text_index_to_buffer(1));
			HString f = long_literal.subview(0, long_literal.text_index_to_buffer(long_buffer.text_length() - 1));

			CHECK_NE(a, b);
			CHECK_NE(a, c);
			CHECK_NE(b, c);
			CHECK_NE(d, e);
			CHECK_NE(d, f);
			CHECK_NE(e, f);
		}
	}

	SUBCASE("str getter") {
		HString a = short_literal;
		HString b = short_buffer;
		HString c = long_literal;
		HString d = long_buffer;

		CHECK_EQ(a.text_length(), short_literal.text_length());
		CHECK_EQ(a.length(), short_literal.length());
		CHECK_EQ(b.text_length(), short_buffer.text_length());
		CHECK_EQ(b.length(), short_buffer.length());
		CHECK_EQ(c.text_length(), long_literal.text_length());
		CHECK_EQ(c.length(), long_literal.length());
		CHECK_EQ(d.text_length(), long_buffer.text_length());
		CHECK_EQ(d.length(), long_buffer.length());

		CHECK_EQ(a.c_str(), a.data());
		CHECK_EQ(b.c_str(), b.data());
		CHECK_EQ(c.c_str(), c.data());
		CHECK_EQ(d.c_str(), d.data());

		CHECK_EQ(a.raw_data(), reinterpret_cast<const char*>(a.data()));
		CHECK_EQ(b.raw_data(), reinterpret_cast<const char*>(b.data()));
		CHECK_EQ(c.raw_data(), reinterpret_cast<const char*>(c.data()));
		CHECK_EQ(d.raw_data(), reinterpret_cast<const char*>(d.data()));
	}

	SUBCASE("memory op") {
		HString str = long_literal;

		// literal clear
		str.clear();
		CHECK(str.empty());
		CHECK_GE(str.capacity(), HString::SSOCapacity);
		CHECK_EQ(str.size(), 0);

		// buffer clear
		str = long_buffer;
		str.clear();
		auto old_capacity = str.capacity();
		auto old_data = str.data();
		CHECK(str.empty());
		CHECK_EQ(str.capacity(), old_capacity);
		CHECK_EQ(str.size(), 0);
		CHECK_EQ(str.data(), old_data);

		// release
		str.release();
		CHECK(str.empty());
		CHECK_EQ(str.size(), 0);

		// release and eq to capacity
		str.reserve(4096);
		str.release(4096);
		CHECK(str.empty());
		CHECK_EQ(str.size(), 0);
		str.release();

		// release with content
		str = long_literal;
		str.release();
		CHECK(str.empty());
		CHECK_EQ(str.size(), 0);

		// release with reserve size
		str = long_literal;
		str.release(10086);
		CHECK(str.empty());
		CHECK_GE(str.capacity(), 10086);
		CHECK_EQ(str.size(), 0);

		// reserve
		str.release();
		str.reserve(60);
		CHECK(str.empty());
		CHECK_EQ(str.size(), 0);

		// shrink
		CHECK(str.empty());
		CHECK_EQ(str.size(), 0);

		// shrink with content
		str = long_literal;
		str.reserve(10086);
		CHECK_EQ(str.size(), long_literal.size());
		CHECK_EQ(str, long_literal);

		// resize
		str.release();
		str = long_literal;
		str.resize(200, u8'g');
		CHECK_EQ(str.size(), 200);
		for (size_t i = 0; i < long_literal.size(); ++i) {
			CHECK_EQ(str.at(i), long_literal.at(i));
		}
		for (size_t i = long_literal.size(); i < 200; ++i) {
			CHECK_EQ(str.at(i), u8'g');
		}

		// resize unsafe
		str.release();
		str = long_literal;
		str.resize(200);
		CHECK_EQ(str.size(), 200);
		for (size_t i = 0; i < long_literal.size(); ++i) {
			CHECK_EQ(str.at(i), long_literal.at(i));
		}

		// resize default
		str.release();
		str = long_literal;
		str.resize(200, {});
		CHECK_EQ(str.size(), 200);
		for (size_t i = 0; i < long_literal.size(); ++i) {
			CHECK_EQ(str.at(i), long_literal.at(i));
		}
		for (size_t i = long_literal.size(); i < 200; ++i) {
			CHECK_EQ(str.at(i), 0);
		}

		// resize zeroed
		str.release();
		str = long_literal;
		str.resize(200, 0);
		CHECK_EQ(str.size(), 200);
		for (size_t i = 0; i < long_literal.size(); ++i) {
			CHECK_EQ(str.at(i), long_literal.at(i));
		}
		for (size_t i = long_literal.size(); i < 200; ++i) {
			CHECK_EQ(str.at(i), 0);
		}
	}

	SUBCASE("add") {
		HString str = long_literal;

		// add
		str.push_back(u8'g');
		CHECK_EQ(str.size(), long_literal.size() + 1);
		CHECK_GE(str.capacity(), long_literal.size() + 1);
		CHECK_EQ(str.at(long_literal.size()), u8'g');
		for (size_t i = 0; i < long_literal.size(); ++i) {
			CHECK_EQ(str.at(i), long_literal.at(i));
		}
		CHECK_EQ(str.at(long_literal.size()), u8'g');

		// add unsafe
		str.append(10, 0);
		CHECK_EQ(str.size(), long_literal.size() + 1 + 10);
		CHECK_GE(str.capacity(), long_literal.size() + 1 + 10);
		for (size_t i = 0; i < long_literal.size(); ++i) {
			CHECK_EQ(str.at(i), long_literal.at(i));
		}
		CHECK_EQ(str.at(long_literal.size()), u8'g');
		// for (size_t i = long_literal.size() + 1; i < long_literal.size() + 1 + 10; ++i)
		// {
		//     CHECK_EQ(str.at(i), 0);
		// }

		// add default
		str.append(10, 0);
		CHECK_EQ(str.size(), long_literal.size() + 1 + 10 + 10);
		CHECK_GE(str.capacity(), long_literal.size() + 1 + 10 + 10);
		for (size_t i = 0; i < long_literal.size(); ++i) {
			CHECK_EQ(str.at(i), long_literal.at(i));
		}
		CHECK_EQ(str.at(long_literal.size()), u8'g');
		// for (size_t i = long_literal.size() + 1; i < long_literal.size() + 1 + 10; ++i)
		// {
		//     CHECK_EQ(str.at(i), 0);
		// }
		for (size_t i = long_literal.size() + 1 + 10; i < long_literal.size() + 1 + 10 + 10; ++i) {
			CHECK_EQ(str.at(i), 0);
		}

		// add zeroed
		str.append(10, 0);
		CHECK_EQ(str.size(), long_literal.size() + 1 + 10 + 10 + 10);
		CHECK_GE(str.capacity(), long_literal.size() + 1 + 10 + 10 + 10);
		for (size_t i = 0; i < long_literal.size(); ++i) {
			CHECK_EQ(str.at(i), long_literal.at(i));
		}
		CHECK_EQ(str.at(long_literal.size()), u8'g');
		// for (size_t i = long_literal.size() + 1; i < long_literal.size() + 1 + 10; ++i)
		// {
		//     CHECK_EQ(str.at(i), 0);
		// }
		for (size_t i = long_literal.size() + 1 + 10; i < long_literal.size() + 1 + 10 + 10; ++i) {
			CHECK_EQ(str.at(i), 0);
		}
		for (size_t i = long_literal.size() + 1 + 10 + 10; i < long_literal.size() + 1 + 10 + 10 + 10; ++i) {
			CHECK_EQ(str.at(i), 0);
		}
	}

	SUBCASE("add at") {
		HString str = long_literal;

		// add at
		str.insert(0, 1, u8'g');
		CHECK_EQ(str.size(), long_literal.size() + 1);
		CHECK_GE(str.capacity(), long_literal.size() + 1);
		CHECK_EQ(str.at(0), u8'g');
		for (size_t i = 1; i < long_literal.size() + 1; ++i) {
			CHECK_EQ(str.at(i), long_literal.at(i - 1));
		}

		// add at unsafe
		str.insert(0, 10, 0);
		CHECK_EQ(str.size(), long_literal.size() + 1 + 10);
		CHECK_GE(str.capacity(), long_literal.size() + 1 + 10);
		// for (size_t i = 0; i < 10; ++i)
		// {
		//     CHECK_EQ(str.at(i), 0);
		// }
		CHECK_EQ(str.at(10), u8'g');
		for (size_t i = 11; i < long_literal.size() + 1 + 10; ++i) {
			CHECK_EQ(str.at(i), long_literal.at(i - 11));
		}

		// add at default
		str.insert(0, 10, 0);
		CHECK_EQ(str.size(), long_literal.size() + 1 + 10 + 10);
		CHECK_GE(str.capacity(), long_literal.size() + 1 + 10 + 10);
		for (size_t i = 0; i < 10; ++i) {
			CHECK_EQ(str.at(i), 0);
		}
		// for (size_t i = 10; i < 20; ++i)
		// {
		//     CHECK_EQ(str.at(i), 0);
		// }
		CHECK_EQ(str.at(20), u8'g');
		for (size_t i = 21; i < long_literal.size() + 1 + 10 + 10; ++i) {
			CHECK_EQ(str.at(i), long_literal.at(i - 21));
		}

		// add at zeroed
		str.insert(0, 10, 0);
		CHECK_EQ(str.size(), long_literal.size() + 1 + 10 + 10 + 10);
		CHECK_GE(str.capacity(), long_literal.size() + 1 + 10 + 10 + 10);
		for (size_t i = 0; i < 10; ++i) {
			CHECK_EQ(str.at(i), 0);
		}
		for (size_t i = 10; i < 20; ++i) {
			CHECK_EQ(str.at(i), 0);
		}
		// for (size_t i = 20; i < 30; ++i)
		// {
		//     CHECK_EQ(str.at(i), 0);
		// }
		CHECK_EQ(str.at(30), u8'g');
		for (size_t i = 31; i < long_literal.size() + 1 + 10 + 10 + 10; ++i) {
			CHECK_EQ(str.at(i), long_literal.at(i - 31));
		}
	}

	SUBCASE("append") {
		HStringView append_view_a = short_literal;
		HStringView append_view_b = u8"🏀🐓";
		UTF8Seq append_seq_a{U'🏀'};

		HString str = long_literal;

		// append
		auto len_before_append = str.length();
		str.append(append_view_a.data());
		auto len_after_append = str.length();
		CHECK_EQ(str.size(), len_before_append + append_view_a.size());
		CHECK_GE(str.capacity(), len_before_append + append_view_a.size());
		for (size_t i = 0; i < long_literal.size(); ++i) {
			CHECK_EQ(str.at(i), long_literal.at(i));
		}
		for (size_t i = len_before_append; i < len_after_append; ++i) {
			CHECK_EQ(str.at(i), append_view_a.at(i - len_before_append));
		}

		// append with len
		auto len_before_append_len = str.length();
		auto append_len = append_view_b.text_index_to_buffer(1);
		str.append(HStringView{append_view_b.data(), append_len});
		auto len_after_append_len = str.length();
		CHECK_EQ(str.size(), len_before_append_len + append_len);
		CHECK_GE(str.capacity(), len_before_append_len + append_len);
		for (size_t i = 0; i < long_literal.size(); ++i) {
			CHECK_EQ(str.at(i), long_literal.at(i));
		}
		for (size_t i = len_before_append; i < len_after_append; ++i) {
			CHECK_EQ(str.at(i), append_view_a.at(i - len_before_append));
		}
		for (size_t i = len_before_append_len; i < len_after_append_len; ++i) {
			CHECK_EQ(str.at(i), append_view_b.at(i - len_before_append_len));
		}

		// append view
		auto len_before_append_view = str.length();
		str.append(append_view_a);
		auto len_after_append_view = str.length();
		CHECK_EQ(str.size(), len_before_append_view + append_view_a.size());
		CHECK_GE(str.capacity(), len_before_append_view + append_view_a.size());
		for (size_t i = 0; i < long_literal.size(); ++i) {
			CHECK_EQ(str.at(i), long_literal.at(i));
		}
		for (size_t i = len_before_append; i < len_after_append; ++i) {
			CHECK_EQ(str.at(i), append_view_a.at(i - len_before_append));
		}
		for (size_t i = len_before_append_len; i < len_after_append_len; ++i) {
			CHECK_EQ(str.at(i), append_view_b.at(i - len_before_append_len));
		}
		for (size_t i = len_before_append_view; i < len_after_append_view; ++i) {
			CHECK_EQ(str.at(i), append_view_a.at(i - len_before_append_view));
		}

		// append seq
		auto len_before_append_seq = str.length();
		str.append(append_seq_a);
		auto len_after_append_seq = str.length();
		CHECK_EQ(str.size(), len_before_append_seq + append_seq_a.len);
		CHECK_GE(str.capacity(), len_before_append_seq + append_seq_a.len);
		for (size_t i = 0; i < long_literal.size(); ++i) {
			CHECK_EQ(str.at(i), long_literal.at(i));
		}
		for (size_t i = len_before_append; i < len_after_append; ++i) {
			CHECK_EQ(str.at(i), append_view_a.at(i - len_before_append));
		}
		for (size_t i = len_before_append_len; i < len_after_append_len; ++i) {
			CHECK_EQ(str.at(i), append_view_b.at(i - len_before_append_len));
		}
		for (size_t i = len_before_append_view; i < len_after_append_view; ++i) {
			CHECK_EQ(str.at(i), append_view_a.at(i - len_before_append_view));
		}
		for (size_t i = len_before_append_seq; i < len_after_append_seq; ++i) {
			CHECK_EQ(str.at(i), append_seq_a.at(i - len_before_append_seq));
		}
	}

	SUBCASE("append at") {
		HStringView append_view_a = short_literal;
		HStringView append_view_b = u8"🏀🐓";
		UTF8Seq append_seq_a{U'🏀'};

		HString str = long_literal;

		// append at
		str.insert(0, append_view_a.data());
		auto append_at_begin = 0;
		auto append_at_end = append_view_a.size();
		auto append_at_len = str.length();
		CHECK_EQ(str.size(), long_literal.size() + append_view_a.size());
		CHECK_GE(str.capacity(), long_literal.size() + append_view_a.size());
		for (size_t i = append_at_begin; i < append_at_end; ++i) {
			CHECK_EQ(str.at(i), append_view_a.at(i - append_at_begin));
		}
		for (size_t i = append_at_end; i < str.size(); ++i) {
			CHECK_EQ(str.at(i), long_literal.at(i - append_at_end));
		}

		// append at with len
		auto append_len = append_view_b.text_index_to_buffer(1);
		str.insert(0, HStringView{append_view_b.data(), append_len});
		auto append_at_len_begin = 0;
		auto append_at_len_end = append_len;
		auto append_at_len_len = str.length();
		append_at_begin += append_len;
		append_at_end += append_len;
		CHECK_EQ(str.size(), append_at_len + append_len);
		CHECK_GE(str.capacity(), append_at_len + append_len);
		for (size_t i = append_at_len_begin; i < append_at_len_end; ++i) {
			CHECK_EQ(str.at(i), append_view_b.at(i - append_at_len_begin));
		}
		for (size_t i = append_at_begin; i < append_at_end; ++i) {
			CHECK_EQ(str.at(i), append_view_a.at(i - append_at_begin));
		}
		for (size_t i = append_at_end; i < str.size(); ++i) {
			CHECK_EQ(str.at(i), long_literal.at(i - append_at_end));
		}

		// append at view
		str.insert(0, append_view_a);
		auto append_at_view_begin = 0;
		auto append_at_view_end = append_view_a.size();
		auto append_at_view_len = str.length();
		append_at_len_begin += append_view_a.size();
		append_at_len_end += append_view_a.size();
		append_at_begin += append_view_a.size();
		append_at_end += append_view_a.size();
		CHECK_EQ(str.size(), append_at_len_len + append_view_a.size());
		CHECK_GE(str.capacity(), append_at_len_len + append_view_a.size());
		for (size_t i = append_at_view_begin; i < append_at_view_end; ++i) {
			CHECK_EQ(str.at(i), append_view_a.at(i - append_at_view_begin));
		}
		for (size_t i = append_at_len_begin; i < append_at_len_end; ++i) {
			CHECK_EQ(str.at(i), append_view_b.at(i - append_at_len_begin));
		}
		for (size_t i = append_at_begin; i < append_at_end; ++i) {
			CHECK_EQ(str.at(i), append_view_a.at(i - append_at_begin));
		}
		for (size_t i = append_at_end; i < str.size(); ++i) {
			CHECK_EQ(str.at(i), long_literal.at(i - append_at_end));
		}

		// append at seq
		str.insert(0, append_seq_a);
		auto append_at_seq_begin = 0;
		auto append_at_seq_end = append_seq_a.len;
		auto append_at_seq_len = str.length();
		append_at_view_begin += append_seq_a.len;
		append_at_view_end += append_seq_a.len;
		append_at_len_begin += append_seq_a.len;
		append_at_len_end += append_seq_a.len;
		append_at_begin += append_seq_a.len;
		append_at_end += append_seq_a.len;
		CHECK_EQ(str.size(), append_at_view_len + append_seq_a.len);
		CHECK_GE(str.capacity(), append_at_view_len + append_seq_a.len);
		for (size_t i = append_at_seq_begin; i < append_at_seq_end; ++i) {
			CHECK_EQ(str.at(i), append_seq_a.at(i - append_at_seq_begin));
		}
		for (size_t i = append_at_view_begin; i < append_at_view_end; ++i) {
			CHECK_EQ(str.at(i), append_view_a.at(i - append_at_view_begin));
		}
		for (size_t i = append_at_len_begin; i < append_at_len_end; ++i) {
			CHECK_EQ(str.at(i), append_view_b.at(i - append_at_len_begin));
		}
		for (size_t i = append_at_begin; i < append_at_end; ++i) {
			CHECK_EQ(str.at(i), append_view_a.at(i - append_at_begin));
		}
		for (size_t i = append_at_end; i < str.size(); ++i) {
			CHECK_EQ(str.at(i), long_literal.at(i - append_at_end));
		}
	}

	SUBCASE("remove") {
		// remove at
		{
			HStringView view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
			HStringView remove_item_view = u8"🐓";
			HStringView removed_view_1 = u8"🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
			HStringView remove_start_view = u8"🏀";
			HStringView removed_view_2 = u8"🏀🏀🐓🏀🐓🏀🐓🏀🐓🏀";

			HString str = view;

			// remove start
			str.erase(0, remove_item_view.size());
			CHECK_EQ(str.size(), removed_view_1.size());
			CHECK_GE(str.capacity(), view.size());
			CHECK_EQ(str, removed_view_1);

			// remove center
			str.erase(remove_start_view.size(), remove_item_view.size());
			CHECK_EQ(str.size(), removed_view_2.size());
			CHECK_GE(str.capacity(), view.size());
			CHECK_EQ(str, removed_view_2);
		}

		// [view] remove & remove last & remove all
		{
			// HStringView view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
			// HStringView remove_item_view = u8"🐓";
			// HStringView removed_view = u8"🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
			// HStringView removed_last_view = u8"🏀🐓🏀🐓🏀🐓🏀🐓🏀🏀";
			// HStringView removed_all_view = u8"🏀🏀🏀🏀🏀🏀";
			//
			// HString str = view;
			//
			// // remove
			// str.remove(remove_item_view);
			// CHECK_EQ(str.size(), removed_view.size());
			// CHECK_GE(str.capacity(), view.size());
			// CHECK_EQ(str, removed_view);
			//
			// // remove last
			// str.remove_last(remove_item_view);
			// CHECK_EQ(str.size(), removed_last_view.size());
			// CHECK_GE(str.capacity(), view.size());
			// CHECK_EQ(str, removed_last_view);
			//
			// // remove all
			// str.remove_all(remove_item_view);
			// CHECK_EQ(str.size(), removed_all_view.size());
			// CHECK_GE(str.capacity(), view.size());
			// CHECK_EQ(str, removed_all_view);
		}

		// [seq] remove & remove last & remove all
		{
			// HStringView view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
			// UTF8Seq remove_item_seq = {U'🐓'};
			// HStringView removed_view = u8"🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
			// HStringView removed_last_view = u8"🏀🐓🏀🐓🏀🐓🏀🐓🏀🏀";
			// HStringView removed_all_view = u8"🏀🏀🏀🏀🏀🏀";
			//
			// HString str = view;
			//
			// // remove
			// str.remove(remove_item_seq);
			// CHECK_EQ(str.size(), removed_view.size());
			// CHECK_GE(str.capacity(), view.size());
			// CHECK_EQ(str, removed_view);
			//
			// // remove last
			// str.remove_last(remove_item_seq);
			// CHECK_EQ(str.size(), removed_last_view.size());
			// CHECK_GE(str.capacity(), view.size());
			// CHECK_EQ(str, removed_last_view);
			//
			// // remove all
			// str.remove_all(remove_item_seq);
			// CHECK_EQ(str.size(), removed_all_view.size());
			// CHECK_GE(str.capacity(), view.size());
			// CHECK_EQ(str, removed_all_view);
		}

		// [view] [copy] remove & remove last & remove all
		{
			// HStringView view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
			// HStringView remove_item_view = u8"🐓";
			// HStringView removed_view = u8"🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
			// HStringView removed_last_view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🏀";
			// HStringView removed_all_view = u8"🏀🏀🏀🏀🏀🏀";
			//
			// HString str = view;
			//
			// // remove
			// auto removed = str.remove_copy(remove_item_view);
			// CHECK_EQ(removed.size(), removed_view.size());
			// CHECK_EQ(removed, removed_view);
			//
			// // remove last
			// auto removed_last = str.remove_last_copy(remove_item_view);
			// CHECK_EQ(removed_last.size(), removed_last_view.size());
			// CHECK_EQ(removed_last, removed_last_view);
			//
			// // remove all
			// auto removed_all = str.remove_all_copy(remove_item_view);
			// CHECK_EQ(removed_all.size(), removed_all_view.size());
			// CHECK_EQ(removed_all, removed_all_view);
			//
			// CHECK_EQ(str.size(), view.size());
			// CHECK_EQ(str, view);
		}

		// [seq] [copy] remove & remove last & remove all
		{
			// HStringView view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
			// UTF8Seq remove_item_seq = {U'🐓'};
			// HStringView removed_view = u8"🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
			// HStringView removed_last_view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🏀";
			// HStringView removed_all_view = u8"🏀🏀🏀🏀🏀🏀";
			//
			// HString str = view;
			//
			// // remove
			// auto removed = str.remove_copy(remove_item_seq);
			// CHECK_EQ(removed.size(), removed_view.size());
			// CHECK_EQ(removed, removed_view);
			//
			// // remove last
			// auto removed_last = str.remove_last_copy(remove_item_seq);
			// CHECK_EQ(removed_last.size(), removed_last_view.size());
			// CHECK_EQ(removed_last, removed_last_view);
			//
			// // remove all
			// auto removed_all = str.remove_all_copy(remove_item_seq);
			// CHECK_EQ(removed_all.size(), removed_all_view.size());
			// CHECK_EQ(removed_all, removed_all_view);
			//
			// CHECK_EQ(str.size(), view.size());
			// CHECK_EQ(str, view);
		}
	}

	SUBCASE("replace") {
		// // replace
		// {
		//     HStringView view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
		//
		//     HStringView replace_less_from_view = u8"🐓";
		//     HStringView replace_less_to_view   = u8"g";
		//     HStringView replaced_less_view     = u8"g🏀g🏀g🏀g🏀g🏀g🏀";
		//
		//     HStringView replace_eq_from_view = u8"🏀";
		//     HStringView replace_eq_to_view   = u8"🐓";
		//     HStringView replaced_eq_view     = u8"🐓🐓🐓🐓🐓🐓🐓🐓🐓🐓🐓🐓";
		//
		//     HStringView replace_more_from_view = u8"🐓";
		//     HStringView replace_more_to_view   = u8"🐓鸡";
		//     HStringView replaced_more_view     = u8"🐓鸡🏀🐓鸡🏀🐓鸡🏀🐓鸡🏀🐓鸡🏀🐓鸡🏀";
		//
		//     HString str = view;
		//     str.replace(replace_less_from_view, replace_less_to_view);
		//     CHECK_EQ(str.size(), replaced_less_view.size());
		//     CHECK_EQ(str, replaced_less_view);
		//
		//     str = view;
		//     str.replace(replace_eq_from_view, replace_eq_to_view);
		//     CHECK_EQ(str.size(), replaced_eq_view.size());
		//     CHECK_EQ(str, replaced_eq_view);
		//
		//     str = view;
		//     str.replace(replace_more_from_view, replace_more_to_view);
		//     CHECK_EQ(str.size(), replaced_more_view.size());
		//     CHECK_EQ(str, replaced_more_view);
		// }
		//
		// // [ranged] replace
		// {
		//     HStringView   view          = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
		//     size_t replace_start = view.text_index_to_buffer(2);
		//     size_t replace_end   = view.text_index_to_buffer(view.text_length() - 2);
		//     size_t replace_count = replace_end - replace_start;
		//
		//     HStringView replace_less_from_view = u8"🐓";
		//     HStringView replace_less_to_view   = u8"g";
		//     HStringView replaced_less_view     = u8"🐓🏀g🏀g🏀g🏀g🏀🐓🏀";
		//
		//     HStringView replace_eq_from_view = u8"🏀";
		//     HStringView replace_eq_to_view   = u8"🐓";
		//     HStringView replaced_eq_view     = u8"🐓🏀🐓🐓🐓🐓🐓🐓🐓🐓🐓🏀";
		//
		//     HStringView replace_more_from_view = u8"🐓";
		//     HStringView replace_more_to_view   = u8"🐓鸡";
		//     HStringView replaced_more_view     = u8"🐓🏀🐓鸡🏀🐓鸡🏀🐓鸡🏀🐓鸡🏀🐓🏀";
		//
		//     HString str = view;
		//     str.replace(replace_less_from_view, replace_less_to_view, replace_start, replace_count);
		//     CHECK_EQ(str.size(), replaced_less_view.size());
		//     CHECK_EQ(str, replaced_less_view);
		//
		//     str = view;
		//     str.replace(replace_eq_from_view, replace_eq_to_view, replace_start, replace_count);
		//     CHECK_EQ(str.size(), replaced_eq_view.size());
		//     CHECK_EQ(str, replaced_eq_view);
		//
		//     str = view;
		//     str.replace(replace_more_from_view, replace_more_to_view, replace_start, replace_count);
		//     CHECK_EQ(str.size(), replaced_more_view.size());
		//     CHECK_EQ(str, replaced_more_view);
		// }
		//
		// // [copy] replace
		// {
		//     HStringView view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
		//
		//     HStringView replace_less_from_view = u8"🐓";
		//     HStringView replace_less_to_view   = u8"g";
		//     HStringView replaced_less_view     = u8"g🏀g🏀g🏀g🏀g🏀g🏀";
		//
		//     HStringView replace_eq_from_view = u8"🏀";
		//     HStringView replace_eq_to_view   = u8"🐓";
		//     HStringView replaced_eq_view     = u8"🐓🐓🐓🐓🐓🐓🐓🐓🐓🐓🐓🐓";
		//
		//     HStringView replace_more_from_view = u8"🐓";
		//     HStringView replace_more_to_view   = u8"🐓鸡";
		//     HStringView replaced_more_view     = u8"🐓鸡🏀🐓鸡🏀🐓鸡🏀🐓鸡🏀🐓鸡🏀🐓鸡🏀";
		//
		//     HString str           = view;
		//     auto   replaced_less = str.replace_copy(replace_less_from_view, replace_less_to_view);
		//     CHECK_EQ(replaced_less.size(), replaced_less_view.size());
		//     CHECK_EQ(replaced_less, replaced_less_view);
		//
		//     auto replaced_eq = str.replace_copy(replace_eq_from_view, replace_eq_to_view);
		//     CHECK_EQ(replaced_eq.size(), replaced_eq_view.size());
		//     CHECK_EQ(replaced_eq, replaced_eq_view);
		//
		//     auto replaced_more = str.replace_copy(replace_more_from_view, replace_more_to_view);
		//     CHECK_EQ(replaced_more.size(), replaced_more_view.size());
		//     CHECK_EQ(replaced_more, replaced_more_view);
		//
		//     CHECK_EQ(str.size(), view.size());
		//     CHECK_EQ(str, view);
		// }
		//
		// // [copy] [ranged] replace
		// {
		//     HStringView   view          = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
		//     size_t replace_start = view.text_index_to_buffer(2);
		//     size_t replace_end   = view.text_index_to_buffer(view.text_length() - 2);
		//     size_t replace_count = replace_end - replace_start;
		//
		//     HStringView replace_less_from_view = u8"🐓";
		//     HStringView replace_less_to_view   = u8"g";
		//     HStringView replaced_less_view     = u8"🐓🏀g🏀g🏀g🏀g🏀🐓🏀";
		//
		//     HStringView replace_eq_from_view = u8"🏀";
		//     HStringView replace_eq_to_view   = u8"🐓";
		//     HStringView replaced_eq_view     = u8"🐓🏀🐓🐓🐓🐓🐓🐓🐓🐓🐓🏀";
		//
		//     HStringView replace_more_from_view = u8"🐓";
		//     HStringView replace_more_to_view   = u8"🐓鸡";
		//     HStringView replaced_more_view     = u8"🐓🏀🐓鸡🏀🐓鸡🏀🐓鸡🏀🐓鸡🏀🐓🏀";
		//
		//     HString str           = view;
		//     auto   replaced_less = str.replace_copy(replace_less_from_view, replace_less_to_view, replace_start, replace_count);
		//     CHECK_EQ(replaced_less.size(), replaced_less_view.size());
		//     CHECK_EQ(replaced_less, replaced_less_view);
		//
		//     auto replaced_eq = str.replace_copy(replace_eq_from_view, replace_eq_to_view, replace_start, replace_count);
		//     CHECK_EQ(replaced_eq.size(), replaced_eq_view.size());
		//     CHECK_EQ(replaced_eq, replaced_eq_view);
		//
		//     auto replaced_more = str.replace_copy(replace_more_from_view, replace_more_to_view, replace_start, replace_count);
		//     CHECK_EQ(replaced_more.size(), replaced_more_view.size());
		//     CHECK_EQ(replaced_more, replaced_more_view);
		//
		//     CHECK_EQ(str.size(), view.size());
		//     CHECK_EQ(str, view);
		// }
		//
		// // replace range
		// {
		//     HStringView view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
		//
		//     size_t replace_start = view.text_index_to_buffer(2);
		//     size_t replace_end   = view.text_index_to_buffer(view.text_length() - 2);
		//     size_t replace_count = replace_end - replace_start;
		//
		//     HStringView replace_less_to    = u8"鸡鸡鸡";
		//     HStringView replaced_less_view = u8"🐓🏀鸡鸡鸡🐓🏀";
		//
		//     HStringView replace_more_to    = u8"🐓🏀🐓🏀坤❤坤🐓🏀🐓🏀";
		//     HStringView replaced_more_view = u8"🐓🏀🐓🏀🐓🏀坤❤坤🐓🏀🐓🏀🐓🏀";
		//
		//     HString str = view;
		//
		//     // replace less
		//     str.replace_range(replace_less_to, replace_start, replace_count);
		//     CHECK_EQ(str.size(), replaced_less_view.size());
		//     CHECK_EQ(str, replaced_less_view);
		//
		//     // replace more
		//     str = view;
		//     str.replace_range(replace_more_to, replace_start, replace_count);
		//     CHECK_EQ(str.size(), replaced_more_view.size());
		//     CHECK_EQ(str, replaced_more_view);
		// }
		//
		// // [copy] replace range
		// {
		//     HStringView view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
		//
		//     size_t replace_start = view.text_index_to_buffer(2);
		//     size_t replace_end   = view.text_index_to_buffer(view.text_length() - 2);
		//     size_t replace_count = replace_end - replace_start;
		//
		//     HStringView replace_less_to    = u8"鸡鸡鸡";
		//     HStringView replaced_less_view = u8"🐓🏀鸡鸡鸡🐓🏀";
		//
		//     HStringView replace_more_to    = u8"🐓🏀🐓🏀坤❤坤🐓🏀🐓🏀";
		//     HStringView replaced_more_view = u8"🐓🏀🐓🏀🐓🏀坤❤坤🐓🏀🐓🏀🐓🏀";
		//
		//     HString str = view;
		//
		//     // replace less
		//     auto replaced_less = str.replace_range_copy(replace_less_to, replace_start, replace_count);
		//     CHECK_EQ(replaced_less.size(), replaced_less_view.size());
		//     CHECK_EQ(replaced_less, replaced_less_view);
		//
		//     // replace more
		//     auto replaced_more = str.replace_range_copy(replace_more_to, replace_start, replace_count);
		//     CHECK_EQ(replaced_more.size(), replaced_more_view.size());
		//     CHECK_EQ(replaced_more, replaced_more_view);
		//
		//     CHECK_EQ(str.size(), view.size());
		//     CHECK_EQ(str, view);
		// }
	}


	SUBCASE("index & modify") {
		HStringView view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";

		// at/last buffer
		{
			HString str = view;
			for (size_t i = 0; i < view.size(); ++i) {
				CHECK_EQ(str.at(i), view.at(i));
			}
		}

		// at/last buffer w
		{
			HString str = view;
			for (size_t i = 0; i < view.size(); ++i) {
				CHECK_EQ(str.at(i), view.at(i));
			}
			for (size_t i = 0; i < view.size() / 2; ++i) {
				str.at(i) = u8'g';
			}
			for (size_t i = 0; i < view.size() / 2; ++i) {
				CHECK_EQ(str.at(i), u8'g');
			}
		}

		// at/last text
		{
			HString str = view;
			for (size_t i = 0; i < view.text_length(); ++i) {
				CHECK_EQ(str.at_text(i), view.at_text(i));
				CHECK_EQ(str.last_text(i), view.last_text(i));
			}
		}
	}

	SUBCASE("sub string") {
		HStringView view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";

		// sub string
		{
			size_t sub_start = view.text_index_to_buffer(2);
			size_t sub_end = view.text_index_to_buffer(view.text_length() - 2);
			size_t sub_count = sub_end - sub_start;
			size_t first_count = sub_start - 0;
			size_t last_count = view.size() - sub_end;

			HStringView sub_view = view.subview(sub_start, sub_count);
			HStringView first_view = view.first_view(first_count);
			HStringView last_view = view.last_view(last_count);

			HString str = view;
			str = str.substr(sub_start, sub_count);
			CHECK_EQ(str.size(), sub_view.size());
			CHECK_EQ(str, sub_view);

			str = view;
			str = str.first_str(first_count);
			CHECK_EQ(str.size(), first_view.size());
			CHECK_EQ(str, first_view);

			str = view;
			str = str.last_str(last_count);
			CHECK_EQ(str.size(), last_view.size());
			CHECK_EQ(str, last_view);
		}

		// [copy] sub string
		{
			size_t sub_start = view.text_index_to_buffer(2);
			size_t sub_end = view.text_index_to_buffer(view.text_length() - 2);
			size_t sub_count = sub_end - sub_start;
			size_t first_count = sub_start - 0;
			size_t last_count = view.size() - sub_end;

			HStringView sub_view = view.subview(sub_start, sub_count);
			HStringView first_view = view.first_view(first_count);
			HStringView last_view = view.last_view(last_count);

			HString str = view;

			auto sub_str = str.substr(sub_start, sub_count);
			CHECK_EQ(sub_str.size(), sub_view.size());
			CHECK_EQ(sub_str, sub_view);

			auto first_str = str.first_str(first_count);
			CHECK_EQ(first_str.size(), first_view.size());
			CHECK_EQ(first_str, first_view);

			auto last_str = str.last_str(last_count);
			CHECK_EQ(last_str.size(), last_view.size());
			CHECK_EQ(last_str, last_view);

			CHECK_EQ(str.size(), view.size());
			CHECK_EQ(str, view);
		}
	}

	SUBCASE("sub view") {
		HStringView view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";

		// sub view
		{
			size_t sub_start = view.text_index_to_buffer(2);
			size_t sub_end = view.text_index_to_buffer(view.text_length() - 2);
			size_t sub_count = sub_end - sub_start;
			size_t first_count = sub_start - 0;
			size_t last_count = view.size() - sub_end;

			HStringView sub_view = view.subview(sub_start, sub_count);
			HStringView first_view = view.first_view(first_count);
			HStringView last_view = view.last_view(last_count);

			HString str = view;
			CHECK_EQ(str.substr(sub_start, sub_count), sub_view);
			CHECK_EQ(str.first_view(first_count), first_view);
			CHECK_EQ(str.last_view(last_count), last_view);
		}
	}

	SUBCASE("remove prefix & suffix") {
		// remove prefix & suffix
		{
			HStringView view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";

			HStringView prefix_view = u8"🐓🏀";
			HStringView bad_prefix_view = u8"🏀🐓";
			HStringView suffix_view = u8"🐓🏀";
			HStringView bad_suffix_view = u8"🏀🐓";
			UTF8Seq prefix_seq{U'🐓'};
			UTF8Seq bad_prefix_seq{U'🏀'};
			UTF8Seq suffix_seq{U'🏀'};
			UTF8Seq bad_suffix_seq{U'🐓'};

			HStringView remove_prefix_view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
			HStringView remove_bad_prefix_view = view;
			HStringView remove_suffix_view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
			HStringView remove_bad_suffix_view = view;
			HStringView remove_prefix_seq_view = u8"🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
			HStringView remove_bad_prefix_seq_view = view;
			HStringView remove_suffix_seq_view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓";
			HStringView remove_bad_suffix_seq_view = view;

			HString str = view;

			// remove prefix
			str.remove_prefix(prefix_view);
			CHECK_EQ(str.size(), remove_prefix_view.size());
			CHECK_EQ(str, remove_prefix_view);

			// remove bad prefix
			str = view;
			str.remove_prefix(bad_prefix_view);
			CHECK_EQ(str.size(), remove_bad_prefix_view.size());
			CHECK_EQ(str, remove_bad_prefix_view);

			// remove suffix
			str = view;
			str.remove_suffix(suffix_view);
			CHECK_EQ(str.size(), remove_suffix_view.size());
			CHECK_EQ(str, remove_suffix_view);

			// remove bad suffix
			str = view;
			str.remove_suffix(bad_suffix_view);
			CHECK_EQ(str.size(), remove_bad_suffix_view.size());
			CHECK_EQ(str, remove_bad_suffix_view);

			// remove prefix seq
			str = view;
			str.remove_prefix(prefix_seq);
			CHECK_EQ(str.size(), remove_prefix_seq_view.size());
			CHECK_EQ(str, remove_prefix_seq_view);

			// remove bad prefix seq
			str = view;
			str.remove_prefix(bad_prefix_seq);
			CHECK_EQ(str.size(), remove_bad_prefix_seq_view.size());
			CHECK_EQ(str, remove_bad_prefix_seq_view);

			// remove suffix seq
			str = view;
			str.remove_suffix(suffix_seq);
			CHECK_EQ(str.size(), remove_suffix_seq_view.size());
			CHECK_EQ(str, remove_suffix_seq_view);

			// remove bad suffix seq
			str = view;
			str.remove_suffix(bad_suffix_seq);
			CHECK_EQ(str.size(), remove_bad_suffix_seq_view.size());
			CHECK_EQ(str, remove_bad_suffix_seq_view);
		}

		// [copy] remove prefix & suffix
		{
			HStringView view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";

			HStringView prefix_view = u8"🐓🏀";
			HStringView bad_prefix_view = u8"🏀🐓";
			HStringView suffix_view = u8"🐓🏀";
			HStringView bad_suffix_view = u8"🏀🐓";
			UTF8Seq prefix_seq{U'🐓'};
			UTF8Seq bad_prefix_seq{U'🏀'};
			UTF8Seq suffix_seq{U'🏀'};
			UTF8Seq bad_suffix_seq{U'🐓'};

			HStringView remove_prefix_view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
			HStringView remove_bad_prefix_view = view;
			HStringView remove_suffix_view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
			HStringView remove_bad_suffix_view = view;
			HStringView remove_prefix_seq_view = u8"🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
			HStringView remove_bad_prefix_seq_view = view;
			HStringView remove_suffix_seq_view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓";
			HStringView remove_bad_suffix_seq_view = view;

			HString str = view;

			// remove prefix
			auto removed_prefix = str.RemovePrefix(prefix_view);
			CHECK_EQ(removed_prefix.size(), remove_prefix_view.size());
			CHECK_EQ(removed_prefix, remove_prefix_view);

			// remove bad prefix
			auto removed_bad_prefix = str.RemovePrefix(bad_prefix_view);
			CHECK_EQ(removed_bad_prefix.size(), remove_bad_prefix_view.size());
			CHECK_EQ(removed_bad_prefix, remove_bad_prefix_view);

			// remove suffix
			auto removed_suffix = str.RemoveSuffix(suffix_view);
			CHECK_EQ(removed_suffix.size(), remove_suffix_view.size());
			CHECK_EQ(removed_suffix, remove_suffix_view);

			// remove bad suffix
			auto removed_bad_suffix = str.RemoveSuffix(bad_suffix_view);
			CHECK_EQ(removed_bad_suffix.size(), remove_bad_suffix_view.size());
			CHECK_EQ(removed_bad_suffix, remove_bad_suffix_view);

			// remove prefix seq
			auto removed_prefix_seq = str.RemovePrefix(prefix_seq);
			CHECK_EQ(removed_prefix_seq.size(), remove_prefix_seq_view.size());
			CHECK_EQ(removed_prefix_seq, remove_prefix_seq_view);

			// remove bad prefix seq
			auto removed_bad_prefix_seq = str.RemovePrefix(bad_prefix_seq);
			CHECK_EQ(removed_bad_prefix_seq.size(), remove_bad_prefix_seq_view.size());
			CHECK_EQ(removed_bad_prefix_seq, remove_bad_prefix_seq_view);

			// remove suffix seq
			auto removed_suffix_seq = str.RemoveSuffix(suffix_seq);
			CHECK_EQ(removed_suffix_seq.size(), remove_suffix_seq_view.size());
			CHECK_EQ(removed_suffix_seq, remove_suffix_seq_view);

			// remove bad suffix seq
			auto removed_bad_suffix_seq = str.RemoveSuffix(bad_suffix_seq);
			CHECK_EQ(removed_bad_suffix_seq.size(), remove_bad_suffix_seq_view.size());
			CHECK_EQ(removed_bad_suffix_seq, remove_bad_suffix_seq_view);

			CHECK_EQ(str.size(), view.size());
			CHECK_EQ(str, view);
		}
	}

	SUBCASE("trim") {
		// trim
		{
			HStringView view = u8"鸡鸡鸡  \t\t🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀\t\t  鸡鸡鸡";

			HStringView trim_chs = u8" \t鸡";
			UTF8Seq trim_seq = {U'鸡'};

			HStringView trim_view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
			HStringView trim_start_view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀\t\t  鸡鸡鸡";
			HStringView trim_end_view = u8"鸡鸡鸡  \t\t🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";

			HStringView trim_seq_view = u8"  \t\t🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀\t\t  ";
			HStringView trim_start_seq_view = u8"  \t\t🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀\t\t  鸡鸡鸡";
			HStringView trim_end_seq_view = u8"鸡鸡鸡  \t\t🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀\t\t  ";

			HString str = view;

			// trim
			str.trim(trim_chs);
			CHECK_EQ(str.size(), trim_view.size());
			CHECK_EQ(str, trim_view);

			// trim start
			str = view;
			str.trim_start(trim_chs);
			CHECK_EQ(str.size(), trim_start_view.size());
			CHECK_EQ(str, trim_start_view);

			// trim end
			str = view;
			str.trim_end(trim_chs);
			CHECK_EQ(str.size(), trim_end_view.size());
			CHECK_EQ(str, trim_end_view);

			// trim seq
			str = view;
			str.trim(trim_seq);
			CHECK_EQ(str.size(), trim_seq_view.size());
			CHECK_EQ(str, trim_seq_view);

			// trim start seq
			str = view;
			str.trim_start(trim_seq);
			CHECK_EQ(str.size(), trim_start_seq_view.size());
			CHECK_EQ(str, trim_start_seq_view);

			// trim end seq
			str = view;
			str.trim_end(trim_seq);
			CHECK_EQ(str.size(), trim_end_seq_view.size());
			CHECK_EQ(str, trim_end_seq_view);
		}

		// [copy] trim
		{
			HStringView view = u8"鸡鸡鸡  \t\t🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀\t\t  鸡鸡鸡";

			HStringView trim_chs = u8" \t鸡";
			UTF8Seq trim_seq = {U'鸡'};

			HStringView trim_view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
			HStringView trim_start_view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀\t\t  鸡鸡鸡";
			HStringView trim_end_view = u8"鸡鸡鸡  \t\t🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";

			HStringView trim_seq_view = u8"  \t\t🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀\t\t  ";
			HStringView trim_start_seq_view = u8"  \t\t🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀\t\t  鸡鸡鸡";
			HStringView trim_end_seq_view = u8"鸡鸡鸡  \t\t🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀\t\t  ";

			HString str = view;

			// trim
			auto trimmed = str.Trim(trim_chs);
			CHECK_EQ(trimmed.size(), trim_view.size());
			CHECK_EQ(trimmed, trim_view);

			// trim start
			auto trimmed_start = str.TrimStart(trim_chs);
			CHECK_EQ(trimmed_start.size(), trim_start_view.size());
			CHECK_EQ(trimmed_start, trim_start_view);

			// trim end
			auto trimmed_end = str.TrimEnd(trim_chs);
			CHECK_EQ(trimmed_end.size(), trim_end_view.size());
			CHECK_EQ(trimmed_end, trim_end_view);

			// trim seq
			auto trimmed_seq = str.Trim(trim_seq);
			CHECK_EQ(trimmed_seq.size(), trim_seq_view.size());
			CHECK_EQ(trimmed_seq, trim_seq_view);

			// trim start seq
			auto trimmed_start_seq = str.TrimStart(trim_seq);
			CHECK_EQ(trimmed_start_seq.size(), trim_start_seq_view.size());
			CHECK_EQ(trimmed_start_seq, trim_start_seq_view);

			// trim end seq
			auto trimmed_end_seq = str.TrimEnd(trim_seq);
			CHECK_EQ(trimmed_end_seq.size(), trim_end_seq_view.size());
			CHECK_EQ(trimmed_end_seq, trim_end_seq_view);

			CHECK_EQ(str.size(), view.size());
			CHECK_EQ(str, view);
		}
	}

	SUBCASE("trim invalid") {
		// trim invalid
		{
			HStringView raw_view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
			HStringView view = raw_view.subview(1, raw_view.size() - 2);

			HStringView trim_view = u8"🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓";
			HStringView trim_start_view = view.subview(3);
			HStringView trim_end_view = view.subview(0, view.size() - 3);

			HString str = view;

			// trim
			str.trim_invalid();
			CHECK_EQ(str.size(), trim_view.size());
			CHECK_EQ(str, trim_view);

			// trim start
			str = view;
			str.trim_invalid_start();
			CHECK_EQ(str.size(), trim_start_view.size());
			CHECK_EQ(str, trim_start_view);

			// trim end
			str = view;
			str.trim_invalid_end();
			CHECK_EQ(str.size(), trim_end_view.size());
			CHECK_EQ(str, trim_end_view);
		}

		// [copy] trim invalid
		{
			HStringView raw_view = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
			HStringView view = raw_view.subview(1, raw_view.size() - 2);

			HStringView trim_view = u8"🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓";
			HStringView trim_start_view = view.subview(3);
			HStringView trim_end_view = view.subview(0, view.size() - 3);

			HString str = view;

			// trim
			auto trimmed = str.TrimInvalid();
			CHECK_EQ(trimmed.size(), trim_view.size());
			CHECK_EQ(trimmed, trim_view);

			// trim start
			auto trimmed_start = str.TrimInvalidStart();
			CHECK_EQ(trimmed_start.size(), trim_start_view.size());
			CHECK_EQ(trimmed_start, trim_start_view);

			// trim end
			auto trimmed_end = str.TrimInvalidEnd();
			CHECK_EQ(trimmed_end.size(), trim_end_view.size());
			CHECK_EQ(trimmed_end, trim_end_view);

			CHECK_EQ(str.size(), view.size());
			CHECK_EQ(str, view);
		}
	}

	SUBCASE("partition") {
		// test split by view
		{
			HStringView partition_view = u8"鸡鸡鸡";

			// test normal
			{
				HStringView normal_view = u8"🐓🏀🐓🏀🐓🏀鸡鸡鸡🐓🏀🐓🏀🐓🏀";
				HStringView normal_left_view = u8"🐓🏀🐓🏀🐓🏀";
				HStringView normal_mid_view = u8"鸡鸡鸡";
				HStringView normal_right_view = u8"🐓🏀🐓🏀🐓🏀";

				HString normal_str = normal_view;
				auto normal_result = normal_str.partition(partition_view);
				CHECK_EQ(normal_result[0], normal_left_view);
				CHECK_EQ(normal_result[1], normal_mid_view);
				CHECK_EQ(normal_result[2], normal_right_view);
			}

			// test no left
			{
				HStringView no_left_view = u8"鸡鸡鸡🐓🏀🐓🏀🐓🏀";
				HStringView no_left_left_view = {};
				HStringView no_left_mid_view = u8"鸡鸡鸡";
				HStringView no_left_right_view = u8"🐓🏀🐓🏀🐓🏀";

				HString no_left_str = no_left_view;
				auto no_left_result = no_left_str.partition(partition_view);
				CHECK_EQ(no_left_result[0], no_left_left_view);
				CHECK_EQ(no_left_result[1], no_left_mid_view);
				CHECK_EQ(no_left_result[2], no_left_right_view);
			}

			// test no right
			{
				HStringView no_right_view = u8"🐓🏀🐓🏀🐓🏀鸡鸡鸡";
				HStringView no_right_left_view = u8"🐓🏀🐓🏀🐓🏀";
				HStringView no_right_mid_view = u8"鸡鸡鸡";
				HStringView no_right_right_view = {};

				HString no_right_str = no_right_view;
				auto no_right_result = no_right_str.partition(partition_view);
				CHECK_EQ(no_right_result[0], no_right_left_view);
				CHECK_EQ(no_right_result[1], no_right_mid_view);
				CHECK_EQ(no_right_result[2], no_right_right_view);
			}

			// test failed
			{
				HStringView failed_view = u8"🐓🏀🐓🏀🐓🏀";
				HStringView failed_left_view = failed_view;
				HStringView failed_mid_view = {};
				HStringView failed_right_view = {};

				HString failed_str = failed_view;
				auto failed_result = failed_str.partition(partition_view);
				CHECK_EQ(failed_result[0], failed_left_view);
				CHECK_EQ(failed_result[1], failed_mid_view);
				CHECK_EQ(failed_result[2], failed_right_view);
			}
		}

		// test split by seq
		{
			UTF8Seq partition_seq{U'鸡'};

			// test normal
			{
				HStringView normal_view = u8"🐓🏀🐓🏀🐓🏀鸡🐓🏀🐓🏀🐓🏀";
				HStringView normal_left_view = u8"🐓🏀🐓🏀🐓🏀";
				HStringView normal_mid_view = u8"鸡";
				HStringView normal_right_view = u8"🐓🏀🐓🏀🐓🏀";

				HString normal_str = normal_view;
				auto normal_result = normal_str.partition(partition_seq);
				CHECK_EQ(normal_result[0], normal_left_view);
				CHECK_EQ(normal_result[1], normal_mid_view);
				CHECK_EQ(normal_result[2], normal_right_view);
			}

			// test no left
			{
				HStringView no_left_view = u8"鸡🐓🏀🐓🏀🐓🏀";
				HStringView no_left_left_view = {};
				HStringView no_left_mid_view = u8"鸡";
				HStringView no_left_right_view = u8"🐓🏀🐓🏀🐓🏀";

				HString no_left_str = no_left_view;
				auto no_left_result = no_left_str.partition(partition_seq);
				CHECK_EQ(no_left_result[0], no_left_left_view);
				CHECK_EQ(no_left_result[1], no_left_mid_view);
				CHECK_EQ(no_left_result[2], no_left_right_view);
			}

			// test no right
			{
				HStringView no_right_view = u8"🐓🏀🐓🏀🐓🏀鸡";
				HStringView no_right_left_view = u8"🐓🏀🐓🏀🐓🏀";
				HStringView no_right_mid_view = u8"鸡";
				HStringView no_right_right_view = {};

				HString no_right_str = no_right_view;
				auto no_right_result = no_right_str.partition(partition_seq);
				CHECK_EQ(no_right_result[0], no_right_left_view);
				CHECK_EQ(no_right_result[1], no_right_mid_view);
				CHECK_EQ(no_right_result[2], no_right_right_view);
			}

			// test failed
			{
				HStringView failed_view = u8"🐓🏀🐓🏀🐓🏀";
				HStringView failed_left_view = failed_view;
				HStringView failed_mid_view = {};
				HStringView failed_right_view = {};

				HString failed_str = failed_view;
				auto failed_result = failed_str.partition(partition_seq);
				CHECK_EQ(failed_result[0], failed_left_view);
				CHECK_EQ(failed_result[1], failed_mid_view);
				CHECK_EQ(failed_result[2], failed_right_view);
			}
		}
	}

	SUBCASE("split") {
		HStringView view{u8"This 🐓 is 🐓🐓 a good 🐓 text 🐓"};
		HStringView split_view{u8"🐓"};
		HStringView split_result[] = {
			u8"This ",
			u8" is ",
			u8"",
			u8" a good ",
			u8" text ",
		};
		HStringView split_result_cull_empty[] = {
			u8"This ",
			u8" is ",
			u8" a good ",
			u8" text ",
		};
		HStringView str = view;

		// split to container
		{
			std::vector<HStringView> result;
			uint64_t count;

			result.clear();
			count = str.split(result, split_view);
			CHECK_EQ(count, 5);
			for (uint64_t i = 0; i < count; ++i) {
				CHECK_EQ(result[i], split_result[i]);
			}

			result.clear();
			count = str.split(result, split_view, true);
			CHECK_EQ(count, 4);
			for (uint64_t i = 0; i < count; ++i) {
				CHECK_EQ(result[i], split_result_cull_empty[i]);
			}

			result.clear();
			count = str.split(result, split_view, false, 3);
			CHECK_EQ(count, 3);
			for (uint64_t i = 0; i < count; ++i) {
				CHECK_EQ(result[i], split_result[i]);
			}

			result.clear();
			count = str.split(result, split_view, true, 3);
			CHECK_EQ(count, 3);
			for (uint64_t i = 0; i < count; ++i) {
				CHECK_EQ(result[i], split_result_cull_empty[i]);
			}
		}

		// custom split
		{
			uint64_t count, idx;

			idx = 0;
			count = str.split_each(
				[&](const HStringView& v) {
					CHECK_EQ(v, split_result[idx]);
					++idx;
				},
				split_view);
			CHECK_EQ(count, 5);

			idx = 0;
			count = str.split_each(
				[&](const HStringView& v) {
					CHECK_EQ(v, split_result_cull_empty[idx]);
					++idx;
				},
				split_view,
				true);
			CHECK_EQ(count, 4);

			idx = 0;
			count = str.split_each(
				[&](const HStringView& v) {
					CHECK_EQ(v, split_result[idx]);
					++idx;
				},
				split_view,
				false,
				3);
			CHECK_EQ(count, 3);

			idx = 0;
			count = str.split_each(
				[&](const HStringView& v) {
					CHECK_EQ(v, split_result_cull_empty[idx]);
					++idx;
				},
				split_view,
				true,
				3);
			CHECK_EQ(count, 3);
		}
	}

	SUBCASE("factory") {
		auto raw_c_str = "🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
		auto wide_c_str = L"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
		auto u8_c_str = u8"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
		auto u16_c_str = u"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";
		auto u32_c_str = U"🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀🐓🏀";

		HString reference_str = u8_c_str;

		HString raw_str = raw_c_str;
		HString wide_str = wide_c_str;
		HString u8_str = u8_c_str;
		HString u16_str = u16_c_str;
		HString u32_str = u32_c_str;

		CHECK_EQ(raw_str, reference_str);
		CHECK_EQ(wide_str, reference_str);
		CHECK_EQ(u8_str, reference_str);
		CHECK_EQ(u16_str, reference_str);
		CHECK_EQ(u32_str, reference_str);
	}

	SUBCASE("concat") {
		HString build_a = "🐓🏀🐓🏀";
		HString build_b = U"🐓🏀";
		HString build_c = L"鸡鸡鸡";
		HString result_view = u"🐓🏀🐓🏀🐓🏀鸡鸡鸡";

		HString result = HString::concat(build_a, build_b, build_c);
		CHECK_EQ(result, result_view);
	}

	SUBCASE("join") {
		const char8_t* join_comp_1 = u8"  ";
		const char8_t* join_comp_2 = u8"🐓🏀🐓🏀";
		const char8_t* join_comp_4 = u8"   ";
		const char8_t* join_comp_5 = u8"鸡鸡鸡";
		const char8_t* join_comp_6 = u8"  鸡鸡鸡  ";
		const char8_t* join_comp_7 = u8"  ";
		const char8_t* join_sep = u8",";

		HStringView normal_result_view = u8"  ,🐓🏀🐓🏀,   ,鸡鸡鸡,  鸡鸡鸡  ,  ";
		HStringView skip_empty_result_view = u8"  ,🐓🏀🐓🏀,   ,鸡鸡鸡,  鸡鸡鸡  ,  ";
		HStringView trim_result_view = u8",🐓🏀🐓🏀,,鸡鸡鸡,鸡鸡鸡,";
		HStringView skip_empty_and_trim_result_view = u8"🐓🏀🐓🏀,鸡鸡鸡,鸡鸡鸡";

		std::array<const char8_t*, 6> raw_join_arr = {join_comp_1, join_comp_2, join_comp_4, join_comp_5, join_comp_6, join_comp_7};
		std::array<HStringView, 6> view_join_arr = {join_comp_1, join_comp_2, join_comp_4, join_comp_5, join_comp_6, join_comp_7};
		std::array<HString, 6> str_join_arr = {join_comp_1, join_comp_2, join_comp_4, join_comp_5, join_comp_6, join_comp_7};

		// normal join
		{
			// join str ptr
			{
				HString normal_raw_result = HString::join(raw_join_arr, join_sep, false);
				CHECK_EQ(normal_result_view, normal_raw_result);
			}

			// join view
			{
				HString normal_view_result = HString::join(view_join_arr, join_sep, false);
				CHECK_EQ(normal_result_view, normal_view_result);
			}

			// join str
			{
				HString normal_str_result = HString::join(str_join_arr, join_sep, false);
				CHECK_EQ(normal_result_view, normal_str_result);
			}
		}

		// join with skip empty
		{
			// join str ptr
			{
				HString skip_empty_raw_result = HString::join(raw_join_arr, join_sep, true);
				CHECK_EQ(skip_empty_result_view, skip_empty_raw_result);
			}

			// join view
			{
				HString skip_empty_view_result = HString::join(view_join_arr, join_sep, true);
				CHECK_EQ(skip_empty_result_view, skip_empty_view_result);
			}

			// join str
			{
				HString skip_empty_str_result = HString::join(str_join_arr, join_sep, true);
				CHECK_EQ(skip_empty_result_view, skip_empty_str_result);
			}
		}

		// join with trim
		{
			// join str ptr
			{
				HString trim_raw_result = HString::join(raw_join_arr, join_sep, false, u8" ");
				CHECK_EQ(trim_result_view, trim_raw_result);
			}

			// join view
			{
				HString trim_view_result = HString::join(view_join_arr, join_sep, false, u8" ");
				CHECK_EQ(trim_result_view, trim_view_result);
			}

			// join str
			{
				HString trim_str_result = HString::join(str_join_arr, join_sep, false, u8" ");
				CHECK_EQ(trim_result_view, trim_str_result);
			}
		}

		// join with trim and skip empty
		{
			// join str ptr
			{
				HString skip_empty_and_trim_raw_result = HString::join(raw_join_arr, join_sep, true, u8" ");
				CHECK_EQ(skip_empty_and_trim_result_view, skip_empty_and_trim_raw_result);
			}

			// join view
			{
				HString skip_empty_and_trim_view_result = HString::join(view_join_arr, join_sep, true, u8" ");
				CHECK_EQ(skip_empty_and_trim_result_view, skip_empty_and_trim_view_result);
			}

			// join str
			{
				HString skip_empty_and_trim_str_result = HString::join(str_join_arr, join_sep, true, u8" ");
				CHECK_EQ(skip_empty_and_trim_result_view, skip_empty_and_trim_str_result);
			}
		}
	}
}
