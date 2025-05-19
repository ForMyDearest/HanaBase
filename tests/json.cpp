#include <doctest/doctest.h>

#define HANA_LOG_ENABLE

#include <hana/json.hpp>
#include <hana/log.hpp>

struct JSONTests {
	JSONTests() {
		hana::LogSystem::set_log_file(u8"json.log", true);
	}
};

template<typename T, typename U>
void CHECK_VALUE(const T& x, const U& y) {
	using namespace hana;
	if constexpr (std::is_same_v<T, char8_t*> || std::is_same_v<T, const char8_t*> || std::is_same_v<T, HString> || std::is_same_v<T, HStringView>) {
		CHECK_EQ(HString(x), HString(y));
	} else if constexpr (std::is_floating_point_v<T>) {
		CHECK(std::abs(x- y) < 0.01);
	} else {
		CHECK_EQ(x, y);
	}
}

#define CHECK_ERROR(r, err) CHECK_FALSE(r); CHECK_EQ(r.result, err);

template<typename T>
void CHECK_READ_ERROR(hana::JsonReader& r, hana::HStringView key, hana::JsonResult::ErrorCode code) {
	T value;
	auto result = hana::json_read(r, key, value);
	CHECK_ERROR(result, code);
}

template<typename T>
void TestPrimitiveType(const T& value) {
	using namespace hana;

	JsonWriter writer(1);
	if (true) {
		const char8_t* key = u8"key";
		CHECK(writer.start_object(u8""));
		CHECK(json_write(writer, key, value));
		CHECK(writer.end_object());
	}
	if (true) {
		auto json = writer.dump();
		LOG_INFO(u8"PRIMITIVE JSON: {}", json);

		if constexpr (std::is_same_v<T, HStringView>) {
			JsonReader reader(json);
			CHECK(reader.start_object(u8""));
			HString result;
			CHECK(json_read(reader, u8"key", result));
			CHECK_VALUE(result, value);
			CHECK(reader.end_object());
		} else {
			JsonReader reader(json);
			CHECK(reader.start_object(u8""));
			T result;
			LOG_INFO(u8"{}", typeid(result).name());
			CHECK(json_read(reader, u8"key", result));
			CHECK(reader.end_object());
			CHECK_VALUE(value, result);
		}
	}
}

template<typename T>
struct TestPrimitiveArray {
	template<typename... Args>
	TestPrimitiveArray(Args... params) {
		using namespace hana;
		JsonWriter writer(1);

		std::array<T, sizeof...(Args)> values = {T(params)...};

		if (true) {
			const char8_t* key = u8"key";
			CHECK(writer.start_object(u8""));
			CHECK(json_write(writer, key, values.size(), values.data()));
			CHECK(writer.end_array());
			CHECK(writer.end_object());
		}
		if (true) {
			auto json = writer.dump();
			LOG_INFO(u8"PRIMITIVE ARRAY JSON: {}", json);
			JsonReader reader(json);

			CHECK(reader.start_object(u8""));
			size_t count;
			CHECK(reader.start_array(u8"key", count));

			decltype(values) _values;
			CHECK(json_read(reader, count, _values.data()));
			CHECK(reader.end_array());
			CHECK(reader.end_object());

			for (size_t i = 0; i < std::size(values); i++) {
				CHECK_VALUE(values[i], _values[i]);
			}
		}
	}
};

TEST_CASE_FIXTURE(JSONTests, "primitive") {
	using namespace hana;

	TestPrimitiveType<bool>(true);
	TestPrimitiveType<bool>(false);

	TestPrimitiveType<int8_t>(-1);
	TestPrimitiveType<int16_t>(-1);
	TestPrimitiveType<int32_t>(-1);
	TestPrimitiveType<int64_t>(-1);

	TestPrimitiveType<uint8_t>(-1);
	TestPrimitiveType<uint16_t>(-2);
	TestPrimitiveType<uint32_t>(-3);
	TestPrimitiveType<uint64_t>(-4);

	TestPrimitiveType<float>(234.2f);
	TestPrimitiveType<double>(-3.01e-10);

	TestPrimitiveType<const char8_t*>(u8"😀emoji");
	TestPrimitiveType<HString>(u8"不是哥们");
	TestPrimitiveType<HStringView>(u8"SerdeTest");
}

TEST_CASE_FIXTURE(JSONTests, "array") {
	using namespace hana;

	TestPrimitiveArray<bool>(true, false);
	TestPrimitiveArray<int8_t>(-1, 1, 0);
	TestPrimitiveArray<int16_t>(-1, 1, 0);
	TestPrimitiveArray<int32_t>(-1, 1, 0);
	TestPrimitiveArray<int64_t>(-1, 1, 0);

	TestPrimitiveArray<uint8_t>(-1, 1, 0);
	TestPrimitiveArray<uint16_t>(-1, 1, 0);
	TestPrimitiveArray<uint32_t>(-1, 1, 0);
	TestPrimitiveArray<uint64_t>(-1, 1, 0);

	TestPrimitiveArray<float>(-100.4, 100.001);
	TestPrimitiveArray<double>(-100e10, 100e10);

	TestPrimitiveArray<const char8_t*>(u8"Text", u8"#@@!*&の");
	TestPrimitiveArray<HString>(u8"12345", u8"😀emoji");
	TestPrimitiveArray<HStringView>(u8"eef", u8"积极你太美");
}

TEST_CASE_FIXTURE(JSONTests, "read errors") {
	using namespace hana;
	using enum JsonResult::ErrorCode;

	SUBCASE("NoOpenScope") {
		CHECK_ERROR(JsonReader(u8"{}").end_array(), NoOpenScope);
		CHECK_ERROR(JsonReader(u8"{}").end_object(), NoOpenScope);
	}

	SUBCASE("ScopeTypeMismatch(start_array)") {
		auto obj_reader = JsonReader(u8"{ \"obj\": \"value\" }");
		CHECK(obj_reader.start_object(u8""));
		size_t count;
		CHECK_ERROR(obj_reader.start_array(u8"obj", count), ScopeTypeMismatch);
	}

	SUBCASE("ScopeTypeMismatch(end_objectAsArray)") {
		auto obj_reader = JsonReader(u8"{}");
		CHECK(obj_reader.start_object(u8""));
		CHECK_ERROR(obj_reader.end_array(), ScopeTypeMismatch);
	}

	SUBCASE("ScopeTypeMismatch(start_arrayAsObject)") {
		auto arr_reader = JsonReader(u8"{ \"arr\": [ 0, 1, 2, 3 ] }");
		CHECK(arr_reader.start_object(u8""));
		CHECK_ERROR(arr_reader.start_object(u8"arr"), ScopeTypeMismatch);
	}

	SUBCASE("ScopeTypeMismatch(StartPrimitiveAsObject/Array)") {
		auto value_reader = JsonReader(u8"{ \"value\": 123 }");
		CHECK(value_reader.start_object(u8""));
		CHECK_ERROR(value_reader.start_object(u8"value"), ScopeTypeMismatch);
		size_t count;
		CHECK_ERROR(value_reader.start_array(u8"value", count), ScopeTypeMismatch);

		auto arr_reader = JsonReader(u8"{ \"arr\": [ 0, 1, 2, 3 ] }");
		CHECK(arr_reader.start_object(u8""));
		auto result = arr_reader.start_array(u8"arr", count);
		CHECK(result);
		CHECK_EQ(count, 4);
		CHECK_ERROR(arr_reader.start_object(u8""), ScopeTypeMismatch);
		CHECK_ERROR(arr_reader.start_array(u8"", count), ScopeTypeMismatch);
	}

	SUBCASE("ScopeTypeMismatch(end_arrayAsObject)") {
		auto arr_reader = JsonReader(u8"{ \"arr\": [ 0, 1, 2, 3 ] }");
		CHECK(arr_reader.start_object(u8""));

		size_t count;
		auto result = arr_reader.start_array(u8"arr", count);
		CHECK(result);
		CHECK_EQ(count, 4);
		CHECK_ERROR(arr_reader.end_object(), ScopeTypeMismatch);
	}

	SUBCASE("KeyNotFound(ObjectField)") {
		auto obj_reader = JsonReader(u8"{ \"key\": \"value\" }");
		CHECK(obj_reader.start_object(u8""));
		CHECK_READ_ERROR<HString>(obj_reader, u8"key_mismatch", KeyNotFound);
		size_t count;
		CHECK_ERROR(obj_reader.start_array(u8"key_mismatch",count), KeyNotFound);
		CHECK_ERROR(obj_reader.start_object(u8"key_mismatch"), KeyNotFound);
	}

	SUBCASE("KeyNotFound(ArrayIndexOverflow)") {
		auto arr_reader = JsonReader(u8"{ \"arr\": [ 0 ] }");
		CHECK(arr_reader.start_object(u8""));
		size_t count;
		if (true) {
			auto result = arr_reader.start_array(u8"arr", count);
			CHECK(result);
			CHECK_EQ(count, 1);
		}

		if (true) {
			int32_t value;
			auto result = json_read(arr_reader, u8"", value);
			CHECK(result);
			CHECK_EQ(value, 0);
		}
		CHECK_READ_ERROR<int32_t>(arr_reader, u8"", KeyNotFound);
	}

	SUBCASE("EmptyObjectFieldKey") {
		auto obj_reader = JsonReader(u8"{ \"key\": \"value\" }");
		size_t count;
		CHECK(obj_reader.start_object(u8""));
		CHECK_ERROR(obj_reader.start_object(u8""), EmptyObjectFieldKey);
		CHECK_ERROR(obj_reader.start_array(u8"", count), EmptyObjectFieldKey);

		CHECK_READ_ERROR<bool>(obj_reader, u8"", EmptyObjectFieldKey);
		CHECK_READ_ERROR<int32_t>(obj_reader, u8"", EmptyObjectFieldKey);
		CHECK_READ_ERROR<int64_t>(obj_reader, u8"", EmptyObjectFieldKey);
		CHECK_READ_ERROR<float>(obj_reader, u8"", EmptyObjectFieldKey);
		CHECK_READ_ERROR<double>(obj_reader, u8"", EmptyObjectFieldKey);
		CHECK_READ_ERROR<HString>(obj_reader, u8"", EmptyObjectFieldKey);
	}

	SUBCASE("ArrayElementWithKey") {
		auto arr_reader = JsonReader(u8"{ \"arr\": [ 0 ] }");
		CHECK(arr_reader.start_object(u8""));
		size_t count;
		auto result = arr_reader.start_array(u8"arr", count);
		CHECK(result);
		CHECK_EQ(count, 1);

		CHECK_READ_ERROR<bool>(arr_reader, u8"k", ArrayElementWithKey);
		CHECK_READ_ERROR<int32_t>(arr_reader, u8"k", ArrayElementWithKey);
		CHECK_READ_ERROR<int64_t>(arr_reader, u8"k", ArrayElementWithKey);
		CHECK_READ_ERROR<float>(arr_reader, u8"k", ArrayElementWithKey);
		CHECK_READ_ERROR<double>(arr_reader, u8"k", ArrayElementWithKey);
		CHECK_READ_ERROR<HString>(arr_reader, u8"k", ArrayElementWithKey);
		CHECK_ERROR(arr_reader.start_object(u8"k"), ArrayElementWithKey);
		CHECK_ERROR(arr_reader.start_array(u8"k", count), ArrayElementWithKey);
	}

	SUBCASE("RootObjectWithKey") {
		auto obj_reader = JsonReader(u8"{ \"key\": \"value\" }");
		CHECK_ERROR(obj_reader.start_object(u8"key"), RootObjectWithKey);
	}
}

TEST_CASE_FIXTURE(JSONTests, "write errors") {
	using namespace hana;
	using enum JsonResult::ErrorCode;

	SUBCASE("NoOpenScope") {
		CHECK_ERROR(JsonWriter(1).end_array(), NoOpenScope);
		CHECK_ERROR(JsonWriter(1).end_object(), NoOpenScope);
	}

	SUBCASE("ScopeTypeMismatch") {
		auto obj_writer = JsonWriter(3);
		CHECK(obj_writer.start_object(u8""));
		CHECK(obj_writer.start_object(u8"obj"));
		CHECK_ERROR(obj_writer.end_array(), ScopeTypeMismatch);

		auto arr_writer = JsonWriter(3);
		CHECK(arr_writer.start_object(u8""));
		CHECK(arr_writer.start_array(u8"arr"));
		CHECK_ERROR(arr_writer.end_object(), ScopeTypeMismatch);
	}

	SUBCASE("EmptyObjectFieldKey") {
		auto obj_writer = JsonWriter(3);
		CHECK(obj_writer.start_object(u8""));
		CHECK_ERROR(obj_writer.start_object(u8""), EmptyObjectFieldKey);
		CHECK_ERROR(obj_writer.start_array(u8""), EmptyObjectFieldKey);

		bool b = false;
		int32_t i32 = 0;
		int64_t i64 = 0;
		float f = 0.f;
		double d = 0.0;
		HString s = u8"";
		CHECK_ERROR(json_write(obj_writer, u8"", b), EmptyObjectFieldKey);
		CHECK_ERROR(json_write(obj_writer, u8"", i32), EmptyObjectFieldKey);
		CHECK_ERROR(json_write(obj_writer, u8"", i64), EmptyObjectFieldKey);
		CHECK_ERROR(json_write(obj_writer, u8"", f), EmptyObjectFieldKey);
		CHECK_ERROR(json_write(obj_writer, u8"", d), EmptyObjectFieldKey);
		CHECK_ERROR(json_write(obj_writer, u8"", s), EmptyObjectFieldKey);
	}

	SUBCASE("ArrayElementWithKey") {
		auto arr_writer = JsonWriter(3);
		CHECK(arr_writer.start_object(u8""));
		CHECK(arr_writer.start_array(u8"arr"));
		CHECK_ERROR(arr_writer.start_object(u8"k"), ArrayElementWithKey);
		CHECK_ERROR(arr_writer.start_array(u8"k"), ArrayElementWithKey);

		bool b = false;
		int32_t i32 = 0;
		int64_t i64 = 0;
		float f = 0.f;
		double d = 0.0;
		HString s = u8"";
		CHECK_ERROR(json_write(arr_writer, u8"k", b), ArrayElementWithKey);
		CHECK_ERROR(json_write(arr_writer, u8"k", i32), ArrayElementWithKey);
		CHECK_ERROR(json_write(arr_writer, u8"k", i64), ArrayElementWithKey);
		CHECK_ERROR(json_write(arr_writer, u8"k", f), ArrayElementWithKey);
		CHECK_ERROR(json_write(arr_writer, u8"k", d), ArrayElementWithKey);
		CHECK_ERROR(json_write(arr_writer, u8"k", s), ArrayElementWithKey);
	}

	SUBCASE("RootObjectWithKey") {
		auto obj_writer = JsonWriter(3);
		CHECK_ERROR(obj_writer.start_object(u8"key"), RootObjectWithKey);
	}
}
