#include <hana/log.hpp>
#include <hana/archive/json.hpp>

#include <stack>
#include <vector>

#include <yyjson.h>

namespace hana
{
	using enum JsonResult::ErrorCode;

	const char* CStringBuild(yyjson_mut_doc* doc, HStringView sv) {
		return yyjson_mut_get_str(yyjson_mut_strncpy(doc, sv.raw_data(), sv.size()));
	}

	struct JsonWriterImpl {
		struct Level {
			enum EType {
				kObject,
				kArray
			};

			yyjson_mut_val* value;
			EType type;

			Level(yyjson_mut_val* _value, EType _type) noexcept : value(_value), type(_type) {}
		};

		yyjson_mut_doc* document;
		std::vector<Level> stack;

		explicit JsonWriterImpl(size_t level_depth) {
			document = yyjson_mut_doc_new(nullptr);
			stack.reserve(level_depth);
		}

		~JsonWriterImpl() {
			yyjson_mut_doc_free(document);
		}

		JsonResult start_object(HStringView key) {
			yyjson_mut_val* obj = yyjson_mut_obj(document);

			if (stack.empty()) {
				if (!key.empty()) {
					return ROOT_OBJECT_WITH_KEY;
				}

				yyjson_mut_doc_set_root(document, obj);
				stack.emplace_back(obj, Level::kObject);
				return {};
			}

			const auto level = stack.back();
			if (level.type == Level::kArray) {
				if (!key.empty()) {
					return ARRAY_ELEMENT_WITH_KEY;
				}

				if (yyjson_mut_arr_add_val(level.value, obj)) {
					stack.emplace_back(obj, Level::kObject);
					return {};
				}
			} else {
				if (key.empty()) {
					return EMPTY_OBJECT_FIELD_KEY;
				}

				const char* ckey = CStringBuild(document, key);
				if (yyjson_mut_obj_add_val(document, level.value, ckey, obj)) {
					stack.emplace_back(obj, Level::kObject);
					return {};
				}
			}

			return UNKNOWN_ERROR;
		}

		JsonResult start_array(HStringView key) {
			if (stack.empty()) {
				return NO_OPEN_SCOPE;
			}

			yyjson_mut_val* arr = yyjson_mut_arr(document);
			auto [array,type] = stack.back();
			bool success = false;

			if (type == Level::kArray) {
				if (!key.empty()) {
					return ARRAY_ELEMENT_WITH_KEY;
				}
				success = yyjson_mut_arr_add_val(array, arr);
			} else if (type == Level::kObject) {
				if (key.empty()) {
					return EMPTY_OBJECT_FIELD_KEY;
				}
				const char* ckey = CStringBuild(document, key);
				success = yyjson_mut_obj_add_val(document, array, ckey, arr);
			}

			stack.emplace_back(arr, Level::kArray);
			if (success) return {};
			return UNKNOWN_ERROR;
		}

		JsonResult end(Level::EType type) noexcept {
			if (stack.empty()) {
				return NO_OPEN_SCOPE;
			}
			if (stack.back().type != type) {
				return SCOPE_TYPE_MISMATCH;
			}
			stack.pop_back();
			return {};
		}

		JsonResult end_array() noexcept {
			return end(Level::kArray);
		}

		JsonResult end_object() noexcept {
			return end(Level::kObject);
		}

		[[nodiscard]] HString dump() const {
			auto str = yyjson_mut_write(document, 0, nullptr);
			HString ret = str;
			free(str);
			return ret;
		}

		template<typename T, typename... Args>
		JsonResult json_write(HStringView key, T value, Args&&... args) {
			if (stack.empty()) {
				return NO_OPEN_SCOPE;
			}

			const auto level = stack.back();
			bool success = false;

			if (level.type == Level::kObject) {
				if (key.empty()) {
					return EMPTY_OBJECT_FIELD_KEY;
				}
				const char* ckey = CStringBuild(document, key);

				if constexpr (std::is_same_v<T, bool>) {
					success = yyjson_mut_obj_add_bool(document, level.value, ckey, value);
				} else if constexpr (std::is_same_v<T, int64_t>) {
					success = yyjson_mut_obj_add_sint(document, level.value, ckey, value);
				} else if constexpr (std::is_same_v<T, uint64_t>) {
					success = yyjson_mut_obj_add_uint(document, level.value, ckey, value);
				} else if constexpr (std::is_floating_point_v<T>) {
					success = yyjson_mut_obj_add_real(document, level.value, ckey, value);
				} else {
					success = yyjson_mut_obj_add_strncpy(document, level.value, ckey, value, std::forward<Args>(args)...);
				}
			} else if (level.type == Level::kArray) {
				if (!key.empty()) {
					return ARRAY_ELEMENT_WITH_KEY;
				}

				if constexpr (std::is_same_v<T, bool>) {
					success = yyjson_mut_arr_add_bool(document, level.value, value);
				} else if constexpr (std::is_same_v<T, int64_t>) {
					success = yyjson_mut_arr_add_sint(document, level.value, value);
				} else if constexpr (std::is_same_v<T, uint64_t>) {
					success = yyjson_mut_arr_add_uint(document, level.value, value);
				} else if constexpr (std::is_floating_point_v<T>) {
					success = yyjson_mut_arr_add_real(document, level.value, value);
				} else {
					success = yyjson_mut_arr_add_strncpy(document, level.value, value, std::forward<Args>(args)...);
				}
			}
			if (success) {
				return {};
			}
			return UNKNOWN_ERROR;
		}

		template<typename T, typename... Args>
		JsonResult write_array(HStringView key, size_t count, T&& values, Args&&... args) {
			using U = std::decay_t<T>;

			if (stack.empty()) {
				return NO_OPEN_SCOPE;
			}
			yyjson_mut_val* arr = nullptr;

			if constexpr (std::is_same_v<U, const bool*>) {
				arr = ::yyjson_mut_arr_with_bool(document, values, count);
			} else if constexpr (std::is_same_v<U, const int64_t*>) {
				arr = ::yyjson_mut_arr_with_sint64(document, values, count);
			} else if constexpr (std::is_same_v<U, const uint64_t*>) {
				arr = ::yyjson_mut_arr_with_uint64(document, values, count);
			} else if constexpr (std::is_same_v<U, const double*>) {
				arr = ::yyjson_mut_arr_with_real(document, values, count);
			} else {
				arr = ::yyjson_mut_arr_with_strncpy(document, values, std::forward<Args>(args)..., count);
			}

			const auto level = stack.back();
			bool success = false;

			if (level.type == Level::kArray) {
				if (!key.empty()) {
					return ARRAY_ELEMENT_WITH_KEY;
				}

				success = yyjson_mut_arr_add_val(level.value, arr);
			} else if (level.type == Level::kObject) {
				if (key.empty()) {
					return EMPTY_OBJECT_FIELD_KEY;
				}
				const char* ckey = CStringBuild(document, key);
				success = yyjson_mut_obj_add_val(document, level.value, ckey, arr);
			}

			if (success) {
				stack.emplace_back(arr, Level::kArray);
				return {};
			}
			return UNKNOWN_ERROR;
		}
	};

	struct JsonReaderImpl {
		struct Level {
			enum EType {
				kObject,
				kArray
			};

			yyjson_val* value;
			EType type;
			uint32_t index = 0;

			Level(yyjson_val* _value, EType _type) noexcept : value(_value), type(_type) {}
		};

		yyjson_doc* document;
		std::stack<Level> stack;

		explicit JsonReaderImpl(HStringView json) {
			yyjson_read_err err = {};
			document = yyjson_read_opts(const_cast<char*>(json.raw_data()), json.size(), 0, nullptr, &err);
			if (document == nullptr) {
				LOG_ERROR(u8"Failed to parse JSON: {}, error: {}", json, err.msg);
			}
		}

		~JsonReaderImpl() {
			yyjson_doc_free(document);
		}

		JsonResult start_object(HStringView key) {
			if (stack.empty()) {
				if (!key.empty()) {
					return ROOT_OBJECT_WITH_KEY;
				}

				auto obj = yyjson_doc_get_root(document);
				if (yyjson_get_type(obj) != YYJSON_TYPE_OBJ) {
					return SCOPE_TYPE_MISMATCH;
				}

				stack.emplace(obj, Level::kObject);
				return {};
			}

			auto parent = stack.top().value;
			auto parent_type = yyjson_get_type(parent);

			if (parent_type == YYJSON_TYPE_ARR) {
				if (!key.empty()) {
					return ARRAY_ELEMENT_WITH_KEY;
				}

				auto obj = yyjson_arr_get(parent, stack.top().index++);
				if (!obj) {
					return KEY_NOT_FOUND;
				}

				if (yyjson_get_type(obj) != YYJSON_TYPE_OBJ) {
					return SCOPE_TYPE_MISMATCH;
				}

				stack.emplace(obj, Level::kObject);
				return {};
			}

			if (parent_type == YYJSON_TYPE_OBJ) {
				if (key.empty()) {
					return EMPTY_OBJECT_FIELD_KEY;
				}

				auto obj = yyjson_obj_get(parent, reinterpret_cast<const char*>(key.data()));
				if (!obj) {
					return KEY_NOT_FOUND;
				}

				if (yyjson_get_type(obj) != YYJSON_TYPE_OBJ) {
					return SCOPE_TYPE_MISMATCH;
				}

				stack.emplace(obj, Level::kObject);
				return {};
			}

			return UNKNOWN_ERROR;
		}

		JsonResult start_array(HStringView key, size_t& count) {
			if (stack.empty()) {
				return NO_OPEN_SCOPE;
			}

			auto parent = stack.top().value;
			auto parent_type = yyjson_get_type(parent);

			if (parent_type == YYJSON_TYPE_ARR) {
				if (!key.empty()) {
					return ARRAY_ELEMENT_WITH_KEY;
				}

				auto arr = yyjson_arr_get(parent, stack.top().index++);
				if (!arr) {
					return KEY_NOT_FOUND;
				}

				if (yyjson_get_type(arr) != YYJSON_TYPE_ARR) {
					return SCOPE_TYPE_MISMATCH;
				}

				stack.emplace(arr, Level::kArray);
				count = yyjson_arr_size(arr);
				return {};
			}

			if (parent_type == YYJSON_TYPE_OBJ) {
				if (key.empty()) {
					return EMPTY_OBJECT_FIELD_KEY;
				}
				auto arr = yyjson_obj_get(parent, reinterpret_cast<const char*>(key.data()));
				if (!arr) {
					return KEY_NOT_FOUND;
				}

				if (yyjson_get_type(arr) != YYJSON_TYPE_ARR) {
					return SCOPE_TYPE_MISMATCH;
				}

				stack.emplace(arr, Level::kArray);
				count = yyjson_arr_size(arr);
				return {};
			}

			return UNKNOWN_ERROR;
		}

		JsonResult end(Level::EType type) noexcept {
			if (stack.empty()) {
				return NO_OPEN_SCOPE;
			}
			if (stack.top().type != type) {
				return SCOPE_TYPE_MISMATCH;
			}
			stack.pop();
			return {};
		}

		JsonResult end_array() noexcept {
			return end(Level::kArray);
		}

		JsonResult end_object() noexcept {
			return end(Level::kObject);
		}

		JsonResult check_scope() const noexcept {
			if (stack.empty()) {
				return NO_OPEN_SCOPE;
			}

			if (stack.top().type != Level::kArray) {
				return SCOPE_TYPE_MISMATCH;
			}

			return {};
		}

		template<typename T>
		JsonResult json_read(HStringView key, T& value) {
			if (stack.empty()) {
				return NO_OPEN_SCOPE;
			}

			auto type = stack.top().type;
			auto parent = stack.top().value;
			const char* ckey = reinterpret_cast<const char*>(key.data());
			yyjson_val* found = nullptr;

			if (type == Level::kObject) {
				if (key.empty()) {
					return EMPTY_OBJECT_FIELD_KEY;
				}

				found = yyjson_obj_get(parent, ckey);
				if (!found) {
					return KEY_NOT_FOUND;
				}
			} else if (type == Level::kArray) {
				if (!key.empty()) {
					return ARRAY_ELEMENT_WITH_KEY;
				}

				found = yyjson_arr_get(parent, stack.top().index++);
				if (!found) {
					return KEY_NOT_FOUND;
				}
			}

			if constexpr (std::is_same_v<T, bool>) {
				value = yyjson_get_bool(found);
			} else if constexpr (std::is_same_v<T, int64_t>) {
				value = yyjson_get_sint(found);
			} else if constexpr (std::is_same_v<T, uint64_t>) {
				value = yyjson_get_uint(found);
			} else if constexpr (std::is_same_v<T, double>) {
				value = yyjson_get_real(found);
			} else {
				value = yyjson_get_str(found);
			}
			return {};
		}
	};
}

namespace hana
{
	struct JsonHelper {
		static JsonWriterImpl* get(JsonWriter& w) {
			return w.pimpl;
		}

		static JsonReaderImpl* get(JsonReader& r) {
			return r.pimpl;
		}
	};

	JsonResult json_write(JsonWriter& w, HStringView key, bool value) {
		return JsonHelper::get(w)->json_write(key, value);
	}

	JsonResult json_write(JsonWriter& w, HStringView key, int64_t value) {
		return JsonHelper::get(w)->json_write(key, value);
	}

	JsonResult json_write(JsonWriter& w, HStringView key, uint64_t value) {
		return JsonHelper::get(w)->json_write(key, value);
	}

	JsonResult json_write(JsonWriter& w, HStringView key, double value) {
		return JsonHelper::get(w)->json_write(key, value);
	}

	JsonResult json_write(JsonWriter& w, HStringView key, const char8_t* value, size_t len) {
		return JsonHelper::get(w)->json_write(key, reinterpret_cast<const char*>(value), len);
	}

	JsonResult json_write(JsonWriter& w, HStringView key, size_t count, const bool* values) {
		return JsonHelper::get(w)->write_array(key, count, values);
	}

	JsonResult json_write(JsonWriter& w, HStringView key, size_t count, const int64_t* values) {
		return JsonHelper::get(w)->write_array(key, count, values);
	}

	JsonResult json_write(JsonWriter& w, HStringView key, size_t count, const uint64_t* values) {
		return JsonHelper::get(w)->write_array(key, count, values);
	}

	JsonResult json_write(JsonWriter& w, HStringView key, size_t count, const double* values) {
		return JsonHelper::get(w)->write_array(key, count, values);
	}

	JsonResult json_write(JsonWriter& w, HStringView key, size_t count, const char8_t** values, const size_t* lens) {
		return JsonHelper::get(w)->write_array(key, count, reinterpret_cast<const char**>(values), lens);
	}

	JsonResult json_read(JsonReader& r, HStringView key, bool& value) {
		return JsonHelper::get(r)->json_read(key, value);
	}

	JsonResult json_read(JsonReader& r, HStringView key, int64_t& value) {
		return JsonHelper::get(r)->json_read(key, value);
	}

	JsonResult json_read(JsonReader& r, HStringView key, uint64_t& value) {
		return JsonHelper::get(r)->json_read(key, value);
	}

	JsonResult json_read(JsonReader& r, HStringView key, double& value) {
		return JsonHelper::get(r)->json_read(key, value);
	}

	JsonResult json_read(JsonReader& r, HStringView key, const char8_t*& value) {
		return JsonHelper::get(r)->json_read(key, reinterpret_cast<const char*&>(value));
	}
}

// JsonWriter
namespace hana
{
	JsonWriter::JsonWriter(size_t level_depth) {
		pimpl = new JsonWriterImpl(level_depth);
	}

	JsonWriter::~JsonWriter() noexcept {
		delete pimpl;
	}

	JsonResult JsonWriter::start_object(HStringView key) {
		return pimpl->start_object(key);
	}

	JsonResult JsonWriter::start_array(HStringView key) {
		return pimpl->start_array(key);
	}

	JsonResult JsonWriter::end_array() noexcept {
		return pimpl->end_array();
	}

	JsonResult JsonWriter::end_object() noexcept {
		return pimpl->end_object();
	}

	HString JsonWriter::dump() const {
		return pimpl->dump();
	}
}

// JsonReader
namespace hana
{
	JsonReader::JsonReader(HStringView json) {
		pimpl = new JsonReaderImpl(json);
	}

	JsonReader::~JsonReader() noexcept {
		delete pimpl;
	}

	JsonResult JsonReader::start_object(HStringView key) {
		return pimpl->start_object(key);
	}

	JsonResult JsonReader::start_array(HStringView key, size_t& count) {
		return pimpl->start_array(key, count);
	}

	JsonResult JsonReader::end_array() noexcept {
		return pimpl->end_array();
	}

	JsonResult JsonReader::end_object() noexcept {
		return pimpl->end_object();
	}

	JsonResult JsonReader::check_scope() const noexcept {
		return pimpl->check_scope();
	}
}
