#pragma once

#include "hana/memory/rc.hpp"
#include "hana/container/string.hpp"

namespace hana
{
	struct JsonResult {
		enum ErrorCode: uint8_t {
			OK = 0,

			UNKNOWN_ERROR,       // RW
			NO_OPEN_SCOPE,       // RW
			SCOPE_TYPE_MISMATCH, // RW

			EMPTY_OBJECT_FIELD_KEY, // RW
			ARRAY_ELEMENT_WITH_KEY, // RW
			ROOT_OBJECT_WITH_KEY,   // RW

			KEY_NOT_FOUND, // R
		} result;

		explicit constexpr operator bool() const noexcept { return result == OK; }

		constexpr JsonResult(ErrorCode code = OK) noexcept: result(code) {}
	};

	class HANA_BASE_API JsonWriter : RCUniqueInterface {
	public:
		static RCUnique<JsonWriter> create(size_t level_depth);

		JsonResult start_object(HStringView key);
		JsonResult start_array(HStringView key);
		JsonResult end_array() noexcept;
		JsonResult end_object() noexcept;
		[[nodiscard]] HString dump() const;

	private:
		template<typename> friend class RCUnique;
		void rc_delete() noexcept;

		JsonWriter() = default;
		~JsonWriter() = default;
	};

	class HANA_BASE_API JsonReader : RCUniqueInterface {
	public:
		static RCUnique<JsonReader> create(HStringView json);

		JsonResult start_object(HStringView key);
		JsonResult start_array(HStringView key, size_t& count);
		JsonResult end_array() noexcept;
		JsonResult end_object() noexcept;
		JsonResult check_scope() const noexcept;

	private:
		template<typename> friend class RCUnique;
		void rc_delete() noexcept;

		JsonReader() = default;
		~JsonReader() = default;
	};

	HANA_BASE_API JsonResult json_write(const RCUnique<JsonWriter>& w, HStringView key, bool value);
	HANA_BASE_API JsonResult json_write(const RCUnique<JsonWriter>& w, HStringView key, int64_t value);
	HANA_BASE_API JsonResult json_write(const RCUnique<JsonWriter>& w, HStringView key, uint64_t value);
	HANA_BASE_API JsonResult json_write(const RCUnique<JsonWriter>& w, HStringView key, double value);
	HANA_BASE_API JsonResult json_write(const RCUnique<JsonWriter>& w, HStringView key, const char8_t* value, size_t len);

	HANA_BASE_API JsonResult json_write(const RCUnique<JsonWriter>& w, HStringView key, size_t count, const bool* values);
	HANA_BASE_API JsonResult json_write(const RCUnique<JsonWriter>& w, HStringView key, size_t count, const int64_t* values);
	HANA_BASE_API JsonResult json_write(const RCUnique<JsonWriter>& w, HStringView key, size_t count, const uint64_t* values);
	HANA_BASE_API JsonResult json_write(const RCUnique<JsonWriter>& w, HStringView key, size_t count, const double* values);
	HANA_BASE_API JsonResult json_write(const RCUnique<JsonWriter>& w, HStringView key, size_t count, const char8_t** values, const size_t* lens);

	HANA_BASE_API JsonResult json_read(const RCUnique<JsonReader>& r, HStringView key, bool& value);
	HANA_BASE_API JsonResult json_read(const RCUnique<JsonReader>& r, HStringView key, int64_t& value);
	HANA_BASE_API JsonResult json_read(const RCUnique<JsonReader>& r, HStringView key, uint64_t& value);
	HANA_BASE_API JsonResult json_read(const RCUnique<JsonReader>& r, HStringView key, double& value);
	HANA_BASE_API JsonResult json_read(const RCUnique<JsonReader>& r, HStringView key, const char8_t*& value);

	inline JsonResult json_write(const RCUnique<JsonWriter>& w, HStringView key, int8_t value) { return json_write(w, key, static_cast<int64_t>(value)); }
	inline JsonResult json_write(const RCUnique<JsonWriter>& w, HStringView key, int16_t value) { return json_write(w, key, static_cast<int64_t>(value)); }
	inline JsonResult json_write(const RCUnique<JsonWriter>& w, HStringView key, int32_t value) { return json_write(w, key, static_cast<int64_t>(value)); }
	inline JsonResult json_write(const RCUnique<JsonWriter>& w, HStringView key, uint8_t value) { return json_write(w, key, static_cast<uint64_t>(value)); }
	inline JsonResult json_write(const RCUnique<JsonWriter>& w, HStringView key, uint16_t value) { return json_write(w, key, static_cast<uint64_t>(value)); }
	inline JsonResult json_write(const RCUnique<JsonWriter>& w, HStringView key, uint32_t value) { return json_write(w, key, static_cast<uint64_t>(value)); }
	inline JsonResult json_write(const RCUnique<JsonWriter>& w, HStringView key, float value) { return json_write(w, key, static_cast<double>(value)); }
	inline JsonResult json_write(const RCUnique<JsonWriter>& w, HStringView key, long double value) { return json_write(w, key, static_cast<double>(value)); }
	inline JsonResult json_write(const RCUnique<JsonWriter>& w, HStringView key, HStringView value) { return json_write(w, key, value.data(), value.size()); }
	inline JsonResult json_write(const RCUnique<JsonWriter>& w, HStringView key, const char8_t* value) { return json_write(w, key, HStringView{value}); }

	template<typename T>
	JsonResult json_write(const RCUnique<JsonWriter>& w, HStringView key, size_t count, const T* values) {
		auto result = w->start_array(key);
		if (!result) return result;

		for (size_t i = 0; i < count; i++) {
			if (result = json_write(w, u8"", values[i]); !result) return result;
		}
		return result;
	}

#define HANA_JSON_READ_AS(Type, As)																\
	inline JsonResult json_read(const RCUnique<JsonReader>& r, HStringView key, Type& value) {	\
		As tmp;																					\
		auto result = json_read(r, key, tmp);													\
		if (result) value = tmp;																\
		return result;																			\
	}

	HANA_JSON_READ_AS(int8_t, int64_t)
	HANA_JSON_READ_AS(int16_t, int64_t)
	HANA_JSON_READ_AS(int32_t, int64_t)
	HANA_JSON_READ_AS(uint8_t, uint64_t)
	HANA_JSON_READ_AS(uint16_t, uint64_t)
	HANA_JSON_READ_AS(uint32_t, uint64_t)
	HANA_JSON_READ_AS(float, double)
	HANA_JSON_READ_AS(long double, double)
	HANA_JSON_READ_AS(HStringView, const char8_t*)
	HANA_JSON_READ_AS(HString, const char8_t*)

#undef HANA_JSON_READ_AS

	template<typename T>
	JsonResult json_read(const RCUnique<JsonReader>& r, size_t count, T* values) {
		auto result = r->check_scope();
		if (!result) return result;

		for (auto i = 0; i < count; i++) {
			if (result = json_read(r, u8"", values[i]); !result) return result;
		}
		return {};
	}
}
