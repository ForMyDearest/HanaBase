#pragma once

#include "string.hpp"

#include <locale>

namespace hana::fmt
{
	class format_args;

	template<typename...> class fstring;

	template<typename... Args>
	using format_string = fstring<std::type_identity_t<Args>...>;

	template<typename OutputIt>
	struct format_to_n_result {
		OutputIt out;
		std::iter_difference_t<OutputIt> size;
	};

	struct format_to_result {
		// Pointer to just after the last successful write in the array.
		char8_t* out;
		// Specifies if the output was truncated.
		bool truncated;

		operator char8_t*() const; // NOLINT(*-explicit-constructor)
	};
}

namespace hana
{
#pragma region format_to

	/*!
	 * @brief
	 *		Formats `args` according to specifications in `fmt`, writes the result to
	 *		the output iterator `out` and returns the iterator past the end of the output
	 *		range. `format_to` does not append a terminating null character.
	 *
	 * @code
	 *		auto out = std::vector<char8_t>();
	 *		hana::format_to(std::back_inserter(out), u8"{}", 42);
	 * @endcode
	 */
	template<std::output_iterator<const char8_t&> OutputIt, typename... T>
	OutputIt format_to(OutputIt out, fmt::format_string<T...> fmt, T&&... args);

	template<std::output_iterator<const char8_t&> OutputIt>
	OutputIt vformat_to(OutputIt out, HStringView fmt, fmt::format_args args);

	inline char8_t* vformat_to(char8_t* out, HStringView fmt, fmt::format_args args);

	template<std::output_iterator<const char8_t&> OutputIt, typename... T>
	OutputIt format_to(OutputIt out, const std::locale& loc, fmt::format_string<T...> fmt, T&&... args);

	template<std::output_iterator<const char8_t&> OutputIt>
	OutputIt vformat_to(OutputIt out, const std::locale& loc, HStringView fmt, fmt::format_args args);

	inline char8_t* vformat_to(char8_t* out, const std::locale& loc, HStringView fmt, fmt::format_args args);

#pragma endregion format_to

#pragma region format_to_n

	/*!
	 * @brief
	 *		Formats `args` according to specifications in `fmt`, writes up to `n`
	 *		characters of the result to the output iterator `out` and returns the total
	 *		(not truncated) output size and the iterator past the end of the output
	 *		range. `format_to_n` does not append a terminating null character.
	 */
	template<std::output_iterator<const char8_t&> OutputIt, typename... T>
	fmt::format_to_n_result<OutputIt> format_to_n(OutputIt out, size_t n, fmt::format_string<T...> fmt, T&&... args);

	template<std::output_iterator<const char8_t&> OutputIt>
	fmt::format_to_n_result<OutputIt> vformat_to_n(OutputIt out, size_t n, HStringView fmt, fmt::format_args args);

	inline fmt::format_to_n_result<char8_t*> vformat_to_n(char8_t* out, size_t n, HStringView fmt, fmt::format_args args);

	template<std::output_iterator<const char8_t&> OutputIt, typename... T>
	fmt::format_to_n_result<OutputIt> format_to_n(OutputIt out, size_t n, const std::locale& loc, fmt::format_string<T...> fmt, T&&... args);

	template<std::output_iterator<const char8_t&> OutputIt>
	fmt::format_to_n_result<OutputIt> vformat_to_n(OutputIt out, size_t n, const std::locale& loc, HStringView fmt, fmt::format_args args);

	inline fmt::format_to_n_result<char8_t*> vformat_to_n(char8_t* out, size_t n, const std::locale& loc, HStringView fmt, fmt::format_args args);

#pragma endregion format_to_n

#pragma region format to fixed array

	template<size_t N, typename... T>
	fmt::format_to_result format_to(char8_t (&out)[N], fmt::format_string<T...> fmt, T&&... args);

	template<size_t N>
	fmt::format_to_result vformat_to(char8_t (&out)[N], HStringView fmt, fmt::format_args args);

	template<size_t N, typename... T>
	fmt::format_to_result format_to(char8_t (&out)[N], const std::locale& loc, fmt::format_string<T...> fmt, T&&... args);

	template<size_t N>
	fmt::format_to_result vformat_to(char8_t (&out)[N], const std::locale& loc, HStringView fmt, fmt::format_args args);

#pragma endregion format to fixed array

#pragma region formatted_size

	// Returns the number of chars in the output of `format(fmt, args...)`.
	template<typename... T>
	[[nodiscard]] size_t formatted_size(fmt::format_string<T...> fmt, T&&... args);

	[[nodiscard]] inline size_t vformatted_size(HStringView fmt, fmt::format_args args);

	template<typename... T>
	[[nodiscard]] size_t formatted_size(const std::locale& loc, fmt::format_string<T...> fmt, T&&... args);

	[[nodiscard]] inline size_t vformatted_size(const std::locale& loc, HStringView fmt, fmt::format_args args);

#pragma endregion formatted_size

#pragma region format

	inline HString vformat(HStringView fmt, fmt::format_args args);

	template<typename... T>
	HString format(fmt::format_string<T...> fmt, T&&... args);

	inline HString vformat(const std::locale& loc, HStringView fmt, fmt::format_args args);

	template<typename... T>
	HString format(const std::locale& loc, fmt::format_string<T...> fmt, T&&... args);

#pragma endregion format
}

//================================> fmt <==================================

namespace hana::fmt
{
	class parse_context;
	class context;
	class format_arg;

	//========================> type <==========================

	enum class Type : uint8_t {
		none_type,
		int_type,
		uint_type,
		long_long_type,
		ulong_long_type,
		bool_type,
		char_type,
		last_integer_type = char_type,
		float_type,
		double_type,
		long_double_type,
		last_numeric_type = long_double_type,
		pointer_type,
		cstring_type,
		string_type,
		custom_type,
	};

	struct custom_value;

	struct format_arg_traits {
		// Function template _Type_eraser mirrors the type dispatching mechanism in the construction of basic_format_arg
		// (N4950 [format.arg]). They determine the mapping from "raw" to "erased" argument type for format_arg_store.
		static auto type_erase(bool) -> bool;
		static auto type_erase(int) -> int;
		static auto type_erase(unsigned int) -> unsigned int;
		static auto type_erase(long long) -> long long;
		static auto type_erase(unsigned long long) -> unsigned long long;
		static auto type_erase(char8_t) -> char8_t;
		static auto type_erase(float) -> float;
		static auto type_erase(double) -> double;
		static auto type_erase(long double) -> long double;
		static auto type_erase(const void*) -> const void*;
		static auto type_erase(const char8_t*) -> const char8_t*;
		static auto type_erase(HStringView) -> HStringView;
		static auto type_erase(auto) -> custom_value;

		template<typename T>
		using storage_type = decltype(type_erase(std::declval<std::remove_cvref_t<T>>()));
	};

	class buffer {
		using grow_fun = void (*)(buffer* buf, size_t capacity);

	public:
		using value_type = char8_t;
		using const_reference = const char8_t&;

		constexpr buffer(buffer&&) = default;
		buffer(const buffer&) = delete;
		void operator=(const buffer&) = delete;

		constexpr char8_t& operator[](size_t index) { return ptr_[index]; }
		constexpr char8_t operator[](size_t index) const { return ptr_[index]; }

		constexpr char8_t* begin() noexcept { return ptr_; }
		constexpr const char8_t* begin() const noexcept { return ptr_; }

		constexpr char8_t* end() noexcept { return ptr_ + size_; }
		constexpr const char8_t* end() const noexcept { return ptr_ + size_; }

		constexpr char8_t* data() noexcept { return ptr_; }
		constexpr const char8_t* data() const noexcept { return ptr_; }

		constexpr size_t size() const noexcept { return size_; }
		constexpr size_t capacity() const noexcept { return capacity_; }

		constexpr void clear() {
			size_ = 0;
		}

		constexpr void try_resize(size_t count) {
			try_reserve(count);
			size_ = std::min(count, capacity_);
		}

		constexpr void try_reserve(size_t new_capacity) {
			if (new_capacity > capacity_) grow_(this, new_capacity);
		}

		constexpr void push_back(char8_t value) {
			try_reserve(size_ + 1);
			ptr_[size_++] = value;
		}

		constexpr void append(const char8_t* begin, const char8_t* end) {
			while (begin != end) {
				auto count = static_cast<size_t>(end - begin);
				try_reserve(size_ + count);
				auto free_cap = capacity_ - size_;
				if (free_cap < count) count = free_cap;
				// A loop is faster than memcpy on small sizes.
				char8_t* out = ptr_ + size_;
				for (size_t i = 0; i < count; ++i) out[i] = begin[i];
				size_ += count;
				begin += count;
			}
		}

	protected:
		constexpr buffer(grow_fun grow, size_t sz) noexcept
			: size_(sz), capacity_(sz), grow_(grow) {}

		constexpr explicit buffer(grow_fun grow, char8_t* p = nullptr, size_t sz = 0, size_t cap = 0) noexcept
			: ptr_(p), size_(sz), capacity_(cap), grow_(grow) {}

		constexpr void set(char8_t* buf_data, size_t buf_capacity) noexcept {
			ptr_ = buf_data;
			capacity_ = buf_capacity;
		}

		char8_t* ptr_;
		size_t size_;
		size_t capacity_;
		grow_fun grow_;
	};

	//====================> value store <=======================

	struct custom_value {
		using char_type = char8_t;
		const void* ptr_;
		void (*format_)(parse_context& parse_ctx, context& ctx, const void* arg);

		template<typename T>
		custom_value(const T& value) noexcept;

		constexpr void format(parse_context& parse_ctx, context& format_ctx) const;
	};

	struct value {
		constexpr value() noexcept;
		constexpr value(int val) noexcept;
		constexpr value(unsigned int val) noexcept;
		constexpr value(long long val) noexcept;
		constexpr value(unsigned long long val) noexcept;
		constexpr value(bool val) noexcept;
		constexpr value(char8_t val) noexcept;
		constexpr value(float val) noexcept;
		constexpr value(double val) noexcept;
		constexpr value(long double val) noexcept;
		constexpr value(const void* val) noexcept;
		constexpr value(const char8_t* val) noexcept;
		constexpr value(HStringView val) noexcept;

		template<typename T>
		constexpr value(const T& val) noexcept;

		union {
			std::monostate no_state_;
			int int_state_;
			unsigned int uint_state_;
			long long long_long_state_;
			unsigned long long ulong_long_state_;
			bool bool_state_;
			char8_t char_state_;
			float float_state_;
			double double_state_;
			long double long_double_state_;
			const void* pointer_state_;
			const char8_t* cstring_state_;
			HStringView string_state_;
			custom_value custom_state_;
		};
	};

	enum { packed_arg_bits            = 4 };
	enum { max_packed_args            = 63 / packed_arg_bits };
	enum : uint64_t { is_unpacked_bit = 1ULL << 63 };

	template<size_t NUM_ARGS>
	using arg_t = std::conditional_t<NUM_ARGS <= max_packed_args, value, format_arg>;

	/*!
	 * @brief
	 *		An array of references to arguments. It can be implicitly converted to
	 *		`format_args` for passing into type-erased formatting functions
	 *		such as `vformat`. It is a plain struct to reduce binary size in debug mode.
	 */
	template<size_t NUM_ARGS>
	struct format_arg_store {
		using type = arg_t<NUM_ARGS>[NUM_ARGS];
		type args;
	};

	template<> struct format_arg_store<0> {};

	//===================> format string <======================

	template<typename...>
	class fstring {
	public:
		template<std::convertible_to<HStringView> T>
		consteval fstring(const T& s);

		constexpr HStringView get() const noexcept;

	private:
		HStringView str;
	};

	//=====================> formatter <========================

	enum class Align : uint8_t { none, left, right, center };

	enum class Sign : uint8_t { none, plus, minus, space };

	struct auto_id_tag {};

	struct basic_format_specs {
		int width_ = 0;
		int precision_ = -1;
		char8_t type_ = '\0';
		Align alignment_ = Align::none;
		Sign sgn_ = Sign::none;
		bool alt_ = false;
		bool localized_ = false;
		bool leading_zero_ = false;
		uint8_t fill_length_ = 1;
		// At most one codepoint (so one char32_t or four utf-8 char8_t).
		char8_t fill_[4 / sizeof(char8_t)] = {u8' '};
	};

	/**
	* Adds width and precision references to basic_format_specs.
	* This is required for formatter implementations because we must
	* parse the format specs without having access to the format args (via a format context).
	*/
	struct dynamic_format_specs : basic_format_specs {
		int dynamic_width_index_ = -1;
		int dynamic_precision_index_ = -1;
	};

	// Model of parse_spec_callbacks that fills a basic_format_specs with the parsed data.
	class specs_setter {
	public:
		constexpr explicit specs_setter(basic_format_specs& specs);
		constexpr void on_align(Align aln) const;
		constexpr void on_fill(HStringView sv) const;
		constexpr void on_sign(Sign sgn) const;
		constexpr void on_hash() const;
		constexpr void on_zero() const;
		constexpr void on_width(int width) const;
		constexpr void on_precision(int precision) const;
		constexpr void on_localized() const;
		constexpr void on_type(char8_t type) const;

	protected:
		basic_format_specs& specs_;
	};

	class dynamic_specs_handler : public specs_setter {
	public:
		constexpr dynamic_specs_handler(dynamic_format_specs& specs, parse_context& parse_ctx);
		constexpr void on_dynamic_width(size_t arg_id) const;
		constexpr void on_dynamic_width(auto_id_tag) const;
		constexpr void on_dynamic_precision(size_t arg_id) const;
		constexpr void on_dynamic_precision(auto_id_tag) const;

	private:
		dynamic_format_specs& dynamic_specs_;
		parse_context& parse_ctx_;

		static constexpr int verify_dynamic_arg_index_in_range(size_t idx);
	};
}

namespace hana::fmt
{
	//===============================> struct <=================================

	class format_arg {
	public:
		using handle = custom_value;

		constexpr format_arg() noexcept;
		constexpr format_arg(int val) noexcept;
		constexpr format_arg(unsigned int val) noexcept;
		constexpr format_arg(long long val) noexcept;
		constexpr format_arg(unsigned long long val) noexcept;
		constexpr format_arg(bool val) noexcept;
		constexpr format_arg(char8_t val) noexcept;
		constexpr format_arg(float val) noexcept;
		constexpr format_arg(double val) noexcept;
		constexpr format_arg(long double val) noexcept;
		constexpr format_arg(const void* val) noexcept;
		constexpr format_arg(const char8_t* val) noexcept;
		constexpr format_arg(HStringView val) noexcept;
		template<typename T> constexpr format_arg(const T& val) noexcept;

		constexpr explicit operator bool() const noexcept;

		template<typename Visitor>
		constexpr decltype(auto) visit(Visitor&& vis);

		value value_;
		Type active_state_;
	};

	class format_args {
	public:
		constexpr format_args() noexcept;
		constexpr format_args(format_arg_store<0>, size_t) noexcept;
		HANA_BASE_API format_args(const format_arg* args, size_t count) noexcept;

		template<size_t NUM_ARGS>
		format_args(const format_arg_store<NUM_ARGS>& s, size_t desc);

		HANA_BASE_API format_arg get(size_t index) const noexcept;
		HANA_BASE_API size_t estimate_required_capacity() const noexcept;

	private:
		/**
		 * A descriptor that contains information about formatting arguments.
		 * If the number of arguments is less or equal to max_packed_args then
		 * argument types are passed in the descriptor. This reduces binary code size
		 * per formatting function call.
		 */
		uint64_t desc_;

		union {
			/**
			 * If is_packed() returns true then argument values are stored in values_;
			 * otherwise they are stored in args_. This is done to improve cache
			 * locality and reduce compiled code size since storing larger objects
			 * may require more code (at least on x86-64) even if the same amount of
			 * data is actually copied to stack. It saves ~10% on the bloat test.
			 */
			const value* values_;
			const format_arg* args_;
		};

		HANA_BASE_API Type type(size_t index) const;
		HANA_BASE_API bool is_packed() const;
		HANA_BASE_API uint64_t max_size() const;
	};

	class parse_context {
	public:
		using char_type = char8_t;
		using const_iterator = decltype(HStringView().begin());
		using iterator = const_iterator;

		constexpr explicit parse_context(HStringView fmt, size_t num_args = 0) noexcept;
		parse_context(const parse_context&) = delete;
		parse_context& operator=(const parse_context&) = delete;

		constexpr const_iterator begin() const noexcept;
		constexpr const_iterator end() const noexcept;
		constexpr const char8_t* unchecked_begin() const noexcept;
		constexpr const char8_t* unchecked_end() const noexcept;
		constexpr void advance_to(const const_iterator& iter);

		/**
		 * While the standard presents an exposition-only enum value for
		 * the indexing mode (manual, automatic, or unknown) we use next_arg_id_ to indicate it.
		 * next_arg_id_ > 0 means automatic
		 * next_arg_id_ == 0 means unknown
		 * next_arg_id_ < 0 means manual
		 */
		constexpr size_t next_arg_id();
		constexpr void check_arg_id(size_t id);

	private:
		HStringView format_string_;
		size_t num_args_;
		/**
		 * The standard says this is size_t, however we use ptrdiff_t to save some space
		 * by not having to store the indexing mode. Above is a more detailed explanation
		 * of how this works.
		 */
		ptrdiff_t next_arg_id_ = 0;
	};

	class context {
	public:
		using iterator = std::back_insert_iterator<buffer>;
		using char_type = char8_t;

		context(iterator iter, format_args ctx_args, const std::locale* loc = nullptr);

		format_arg arg(size_t id) const noexcept;
		iterator out() const;
		void advance_to(iterator it);
		const format_args& get_args() const noexcept;
		const std::locale* get_lazy_locale() const;

	protected:
		iterator out_;
		format_args args_;
		const std::locale* loc_;
	};

	template<typename T, Type>
	struct formatter_base {
		constexpr auto parse(parse_context& parse_ctx);
		HANA_BASE_API context::iterator format(T value, context& ctx) const;

	private:
		dynamic_format_specs specs_;
	};
}

namespace hana
{
	//=============================> formatter <================================

	template<typename> struct formatter;
	template<> struct formatter<bool> : fmt::formatter_base<bool, fmt::Type::bool_type> {};
	template<> struct formatter<int> : fmt::formatter_base<int, fmt::Type::int_type> {};
	template<> struct formatter<unsigned int> : fmt::formatter_base<unsigned int, fmt::Type::uint_type> {};
	template<> struct formatter<long long> : fmt::formatter_base<long long, fmt::Type::long_long_type> {};
	template<> struct formatter<unsigned long long> : fmt::formatter_base<unsigned long long, fmt::Type::ulong_long_type> {};
	template<> struct formatter<char8_t> : fmt::formatter_base<char8_t, fmt::Type::char_type> {};
	template<> struct formatter<float> : fmt::formatter_base<float, fmt::Type::float_type> {};
	template<> struct formatter<double> : fmt::formatter_base<double, fmt::Type::double_type> {};
	template<> struct formatter<long double> : fmt::formatter_base<long double, fmt::Type::long_double_type> {};
	template<> struct formatter<const void*> : fmt::formatter_base<const void*, fmt::Type::pointer_type> {};
	template<> struct formatter<const char8_t*> : fmt::formatter_base<const char8_t*, fmt::Type::cstring_type> {};
	template<> struct formatter<HStringView> : fmt::formatter_base<HStringView, fmt::Type::string_type> {};

	template<> struct formatter<signed char> : formatter<int> {};
	template<> struct formatter<short> : formatter<int> {};
	template<> struct formatter<long> : formatter<std::conditional_t<sizeof(long) == sizeof(int), int, long long>> {};
	template<> struct formatter<unsigned char> : formatter<unsigned int> {};
	template<> struct formatter<unsigned short> : formatter<unsigned int> {};
	template<> struct formatter<unsigned long> : formatter<std::conditional_t<sizeof(unsigned long) == sizeof(unsigned int), unsigned int, unsigned long long>> {};
	template<> struct formatter<char> : formatter<char8_t> {};
	template<> struct formatter<void*> : formatter<const void*> {};

	template<> struct formatter<HString> : formatter<HStringView> {};

	template<typename Traits>
	struct formatter<std::basic_string_view<char, Traits>> : formatter<HStringView> {
		using base = formatter<HStringView>;

		fmt::context::iterator format(std::basic_string_view<char, Traits> value, fmt::context& ctx) const {
			return base::format(HStringView{value.data(), value.size()}, ctx);
		}
	};

	template<typename Traits>
	struct formatter<std::basic_string_view<char8_t, Traits>> : formatter<HStringView> {
		using base = formatter<HStringView>;

		fmt::context::iterator format(std::basic_string_view<char8_t, Traits> value, fmt::context& ctx) const {
			return base::format(HStringView{value.data(), value.size()}, ctx);
		}
	};

	template<typename Traits>
	struct formatter<std::basic_string_view<char16_t, Traits>> : formatter<HString> {
		using base = formatter<HString>;

		fmt::context::iterator format(std::basic_string_view<char16_t, Traits> value, fmt::context& ctx) const {
			return base::format(HString{value.data(), value.size()}, ctx);
		}
	};

	template<typename Traits>
	struct formatter<std::basic_string_view<char32_t, Traits>> : formatter<HString> {
		using base = formatter<HString>;

		fmt::context::iterator format(std::basic_string_view<char32_t, Traits> value, fmt::context& ctx) const {
			return base::format(HString{value.data(), value.size()}, ctx);
		}
	};

	template<typename Traits>
	struct formatter<std::basic_string_view<wchar_t, Traits>> : formatter<HString> {
		using base = formatter<HString>;

		fmt::context::iterator format(std::basic_string_view<wchar_t, Traits> value, fmt::context& ctx) const {
			return base::format(HString{value.data(), value.size()}, ctx);
		}
	};

	template<is_char_v Char, typename Traits, typename Allocator>
	struct formatter<std::basic_string<Char, Traits, Allocator>> : formatter<std::basic_string_view<Char, Traits>> {};

	template<is_char_v Char, size_t N>
	struct formatter<Char[N]> : formatter<std::basic_string_view<Char>> {
		using base = formatter<std::basic_string_view<Char>>;

		fmt::context::iterator format(const Char (&value)[N], fmt::context& ctx) const {
			return base::format(std::basic_string_view<Char>{value, N - 1}, ctx);
		}
	};

	template<>
	struct formatter<const char*> : formatter<const char8_t*> {
		using base = formatter<const char8_t*>;

		fmt::context::iterator format(const char* value, fmt::context& ctx) const {
			return base::format(reinterpret_cast<const char8_t*>(value), ctx);
		}
	};

	template<> struct formatter<const char16_t*> : formatter<std::u16string_view> {};
	template<> struct formatter<const char32_t*> : formatter<std::u32string_view> {};
	template<> struct formatter<const wchar_t*> : formatter<std::wstring_view> {};

	template<is_char_v Char>
	struct formatter<Char*> : formatter<const Char*> {};

	template<>
	struct formatter<std::nullptr_t> : formatter<HStringView> {
		using base = formatter<HStringView>;

		fmt::context::iterator format(std::nullptr_t, fmt::context& ctx) const {
			return base::format(HStringView{u8"nullptr"}, ctx);
		}
	};

	template<typename T, typename U = std::remove_const_t<T>>
	concept formattable = requires(formatter<U>& f, const formatter<U>& cf, T&& t, fmt::context fc, fmt::parse_context pc)
	{
		{ f.parse(pc) } -> std::same_as<fmt::parse_context::iterator>;
		{ cf.format(t, fc) } -> std::same_as<fmt::context::iterator>;
		requires std::semiregular<formatter<U>>;
	};
}

namespace hana::fmt
{
	HANA_BASE_API void report_error(const char8_t* message);
	HANA_BASE_API void vformat_to(buffer& buf, HStringView fmt, format_args args, const std::locale* loc = nullptr);
	HANA_BASE_API char8_t* vformat_to(char8_t* out, HStringView fmt, format_args args, const std::locale* loc = nullptr);
	HANA_BASE_API format_to_n_result<char8_t*> vformat_to_n(char8_t* out, size_t n, HStringView fmt, format_args args, const std::locale* loc = nullptr);
	HANA_BASE_API size_t vformatted_size(HStringView fmt, format_args args, const std::locale* loc = nullptr);
	HANA_BASE_API HString vformat(HStringView fmt, format_args args, const std::locale* loc = nullptr);

#pragma region Type

	static_assert(static_cast<int>(Type::custom_type) < 16, "must fit in 4-bit bitfield");

	constexpr bool is_arithmetic_fmt_type(const Type type) {
		return Type::none_type < type && type <= Type::last_numeric_type;
	}

	constexpr bool is_integral_fmt_type(const Type type) {
		return Type::none_type < type && type <= Type::last_integer_type;
	}

	// Maps core type T to the corresponding type enum constant.
	template<typename> struct type_constant : std::integral_constant<Type, Type::custom_type> {};

#define U8FMT_TYPE_CONSTANT(type, constant)\
	template<> struct type_constant<type> : std::integral_constant<Type, Type::constant> {}

	U8FMT_TYPE_CONSTANT(int, int_type);
	U8FMT_TYPE_CONSTANT(unsigned, uint_type);
	U8FMT_TYPE_CONSTANT(long long, long_long_type);
	U8FMT_TYPE_CONSTANT(unsigned long long, ulong_long_type);
	U8FMT_TYPE_CONSTANT(bool, bool_type);
	U8FMT_TYPE_CONSTANT(char8_t, char_type);
	U8FMT_TYPE_CONSTANT(float, float_type);
	U8FMT_TYPE_CONSTANT(double, double_type);
	U8FMT_TYPE_CONSTANT(long double, long_double_type);
	U8FMT_TYPE_CONSTANT(const void*, pointer_type);
	U8FMT_TYPE_CONSTANT(const char8_t*, cstring_type);
	U8FMT_TYPE_CONSTANT(HStringView, string_type);

#undef U8FMT_TYPE_CONSTANT

	template<typename Arg, typename... Args>
	constexpr uint64_t encode_types() {
		uint64_t ret = static_cast<uint64_t>(type_constant<std::decay_t<Arg>>::value);
		if constexpr (sizeof...(Args) != 0) {
			ret |= (encode_types<Args...>() << packed_arg_bits);
		}
		return ret;
	}

	template<typename... T>
	constexpr uint64_t make_descriptor() {
		constexpr size_t NUM_ARGS = sizeof...(T);
		if constexpr (NUM_ARGS == 0) {
			return 0;
		} else if constexpr (NUM_ARGS <= max_packed_args) {
			return encode_types<T...>();
		} else {
			return is_unpacked_bit | NUM_ARGS;
		}
	}

#pragma endregion Type

#pragma region custom_value

	template<typename T>
	custom_value::custom_value(const T& value) noexcept {
		using CT = std::conditional_t<formattable<const T>, const T, T>;
		static_assert(formattable<CT>);

		ptr_ = std::addressof(value);
		format_ = [](parse_context& parse_ctx, context& format_ctx, const void* ptr) {
			using U = std::remove_const_t<T>;
			// doesn't drop const-qualifier per an unnumbered LWG issue
			formatter<U> formatter;
			parse_ctx.advance_to(formatter.parse(parse_ctx));
			format_ctx.advance_to(formatter.format(*const_cast<CT*>(static_cast<const U*>(ptr)), format_ctx));
		};
	}

	constexpr void custom_value::format(parse_context& parse_ctx, context& format_ctx) const {
		format_(parse_ctx, format_ctx, ptr_);
	}

#pragma endregion custom_value

#pragma region value

	constexpr value::value() noexcept: no_state_() {}
	constexpr value::value(int val) noexcept: int_state_(val) {}
	constexpr value::value(unsigned int val) noexcept: uint_state_(val) {}
	constexpr value::value(long long val) noexcept: long_long_state_(val) {}
	constexpr value::value(unsigned long long val) noexcept: ulong_long_state_(val) {}
	constexpr value::value(bool val) noexcept : bool_state_(val) {}
	constexpr value::value(char8_t val) noexcept : char_state_(val) {}
	constexpr value::value(float val) noexcept: float_state_(val) {}
	constexpr value::value(double val) noexcept : double_state_(val) {}
	constexpr value::value(long double val) noexcept: long_double_state_(val) {}
	constexpr value::value(const void* val) noexcept: pointer_state_(val) {}
	constexpr value::value(const char8_t* val) noexcept: cstring_state_(val) {}
	constexpr value::value(HStringView val) noexcept: string_state_(val) {}
	template<typename T> constexpr value::value(const T& val) noexcept: custom_state_(val) {}

#pragma endregion value

#pragma region fstring

	// we need to implement this ourselves because from_chars does not work with wide characters and isn't constexpr
	constexpr const char8_t* parse_nonnegative_integer(const char8_t* first, const char8_t* last, unsigned int& value) {
		assert(first != last && '0' <= *first && *first <= '9');

		constexpr auto max_int = static_cast<unsigned int>(std::numeric_limits<int>::max());
		constexpr auto big_int = max_int / 10u;

		value = 0;

		do {
			if (value > big_int) {
				value = max_int + 1;
				break;
			}
			value = value * 10 + static_cast<unsigned int>(*first - '0');
			++first;
		} while (first != last && '0' <= *first && *first <= '9');

		if (value > max_int) {
			report_error(u8"number is too big");
		}

		return first;
	}

	constexpr const char8_t* parse_nonnegative_integer(const char8_t* first, const char8_t* last, int& value) {
		unsigned int unsigned_value = 0;

		first = parse_nonnegative_integer(first, last, unsigned_value);
		// Never invalid because _Parse_nonnegative_integer throws an error for values that don't fit in signed integers
		value = static_cast<int>(unsigned_value);
		return first;
	}

	template<typename Callbacks>
	constexpr const char8_t* parse_arg_id(const char8_t* first, const char8_t* last, Callbacks&& callbacks) {
		assert(first != last);

		const char8_t ch = *first;
		// No id provided, format string is using automatic indexing.
		if (ch == '}' || ch == ':') {
			callbacks.on_auto_id();
			return first;
		}

		if (ch >= '0' && ch <= '9') {
			unsigned int index = 0;
			// arg_id is not allowed to have any leading zeros, but is allowed to be
			// equal to zero (but not '00'). So if ch is zero we skip the parsing, leave
			// _Index set to zero and let the validity checks below ensure that the arg_id
			// wasn't something like "00", or "023".
			if (ch == '0') {
				++first;
			} else {
				first = parse_nonnegative_integer(first, last, index);
			}

			// The format string shouldn't end right after the index number.
			// The only things permitted after the index are the end of the replacement field ('}')
			// or the beginning of the format spec (':').
			if (first == last || (*first != '}' && *first != ':')) {
				report_error(u8"invalid format string.");
			}

			callbacks.on_manual_id(index);
			return first;
		}
		// This is where we would parse named arg ids if std::format were to support them.
		report_error(u8"invalid format string.");
		abort();
	}

	template<typename Handler>
	constexpr const char8_t* parse_replacement_field(const char8_t* first, const char8_t* last, Handler&& handler) {
		++first;
		if (first == last) {
			report_error(u8"invalid format string.");
		}

		if (*first == '}') {
			// string was "{}", and we have a replacement field
			handler.on_replacement_field(handler.parse_context_.next_arg_id(), first);
		} else if (*first == '{') {
			// string was "{{", so we have a literal "{" to print
			handler.on_text(first, first + 1);
		} else {
			// parse_arg_id expects a handler when it finds an argument id, however
			// parse_replacement_field actually needs to know the value of that argument ID to pass on
			// to Handler.on_replacement_field or Handler.on_format_specs. This parse_arg_id wrapper
			// stores the value of the arg id for later use, so parse_replacement_field has access to it.
			struct IdAdapter {
				parse_context& parse_context_;
				size_t arg_id_ = static_cast<size_t>(-1);

				constexpr void on_auto_id() {
					arg_id_ = parse_context_.next_arg_id();
					assert(arg_id_ != static_cast<size_t>(-1));
				}

				constexpr void on_manual_id(const size_t id) {
					parse_context_.check_arg_id(id);
					arg_id_ = id;
					assert(arg_id_ != static_cast<size_t>(-1));
				}
			} adapter = {handler.parse_context_};

			first = fmt::parse_arg_id(first, last, adapter);
			char8_t ch{};
			if (first != last) {
				ch = *first;
			}

			if (ch == '}') {
				handler.on_replacement_field(adapter.arg_id_, first);
			} else if (ch == ':') {
				first = handler.on_format_specs(adapter.arg_id_, first + 1, last);
				if (first == last || *first != '}') {
					report_error(u8"unknown format specifier.");
				}
			} else {
				report_error(u8"missing '}' in format string.");
			}
		}

		return first + 1;
	}

	template<typename Handler>
	constexpr void parse_format_string(HStringView fmt, Handler&& handler) {
		auto first = fmt.data();
		auto last = first + fmt.size();

		while (first != last) {
			const char8_t* openingCurl = first;
			if (*first != '{') {
				openingCurl = std::find(first, last, u8'{');

				for (;;) {
					const char8_t* closingCurl = std::find(first, openingCurl, u8'}');

					// In this case there are neither closing nor opening curls in [first, _OpeningCurl)
					// Write the whole thing out.
					if (closingCurl == openingCurl) {
						handler.on_text(first, openingCurl);
						break;
					}
					// We know _ClosingCurl isn't past the end because
					// the above condition was not met.
					++closingCurl;
					if (closingCurl == openingCurl || *closingCurl != '}') {
						report_error(u8"unmatched '}' in format string.");
					}
					// We found two closing curls, so output only one of them
					handler.on_text(first, closingCurl);

					// skip over the second closing curl
					first = closingCurl + 1;
				}

				// We are done, there were no replacement fields.
				if (openingCurl == last) {
					return;
				}
			}
			// Parse the replacement field starting at _OpeningCurl and ending sometime before last.
			first = fmt::parse_replacement_field(openingCurl, last, handler);
		}
	}

	template<typename T>
	consteval parse_context::iterator compile_time_parse_format_specs(parse_context& pc) {
		using FormattedTypeMapping = format_arg_traits::storage_type<T>;
		// If the type is going to use a custom formatter we should just use that,
		// instead of trying to instantiate a custom formatter for its erased handle type
		using FormattedType = std::conditional_t<std::is_same_v<FormattedTypeMapping, custom_value>, T, FormattedTypeMapping>;
		return formatter<FormattedType>().parse(pc);
	}

	// set of format parsing actions that only checks for validity
	template<typename... Args>
	struct format_checker {
		using ParseFunc = parse_context::iterator (*)(parse_context&);

		static constexpr size_t NUM_ARGS = sizeof...(Args);
		parse_context parse_context_;
		ParseFunc parse_funcs_[NUM_ARGS > 0 ? NUM_ARGS : 1];

		consteval explicit format_checker(HStringView fmt) noexcept
			: parse_context_(fmt, NUM_ARGS), parse_funcs_{&compile_time_parse_format_specs<Args>...} {}

		static constexpr void on_text(const char8_t*, const char8_t*) noexcept {}
		static constexpr void on_replacement_field(size_t, const char8_t*) noexcept {}

		constexpr const char8_t* on_format_specs(size_t id, const char8_t* first, const char8_t*) {
			parse_context_.advance_to(parse_context_.begin() + (first - std::to_address(parse_context_.begin())));
			if (id < NUM_ARGS) {
				auto iter = parse_funcs_[id](parse_context_); // TRANSITION, VSO-1451773 (workaround: named variable)
				return std::to_address(iter);
			}
			return first;
		}
	};

	template<typename... Args>
	template<std::convertible_to<HStringView> T>
	consteval fstring<Args...>::fstring(const T& s) : str(s) {
		fmt::parse_format_string(str, format_checker<std::remove_cvref_t<Args>...>{str});
	}

	template<typename... Args>
	constexpr HStringView fstring<Args...>::get() const noexcept {
		return str;
	}

#pragma endregion fstring

#pragma region buffer


	struct buffer_traits {
		constexpr explicit buffer_traits(size_t) {}
		static constexpr size_t count() { return 0; }
		static constexpr size_t limit(const size_t size) { return size; }
	};

	class fixed_buffer_traits {
	public:
		constexpr explicit fixed_buffer_traits(const size_t limit) : limit_(limit) {}
		constexpr size_t count() const { return count_; }

		constexpr size_t limit(const size_t size) {
			const size_t n = limit_ > count_ ? limit_ - count_ : 0;
			count_ += size;
			return std::min(size, n);
		}

	private:
		size_t count_ = 0;
		size_t limit_;
	};

	// A buffer that writes to an output iterator when flushed.
	template<typename OutputIt, typename Traits>
	class iterator_buffer : public Traits, public buffer {
	public:
		explicit constexpr iterator_buffer(OutputIt out, size_t n = buffer_size)
			: Traits(n), buffer(grow, data_, 0, buffer_size), out_(out) {}

		constexpr iterator_buffer(iterator_buffer&& other) noexcept
			: Traits(other), buffer(grow, data_, 0, buffer_size), out_(other.out_) {}

		constexpr ~iterator_buffer() {
			// Don't crash if flush fails during unwinding.
			try { flush(); } catch (...) {}
		}

		constexpr OutputIt out() {
			flush();
			return out_;
		}

		constexpr size_t count() const {
			return Traits::count() + this->size();
		}

	private:
		enum { buffer_size = 256 };
		char8_t data_[buffer_size] = {};
		OutputIt out_;

		static constexpr void grow(buffer* buf, size_t) {
			if (buf->size() == buffer_size) reinterpret_cast<iterator_buffer*>(buf)->flush();
		}

		constexpr void flush() {
			auto size = this->size();
			this->clear();
			const char8_t* begin = data_;
			const char8_t* end = begin + this->limit(size);
			while (begin != end) *out_++ = *begin++;
		}
	};

#pragma endregion buffer

#pragma region specs_setter

	constexpr specs_setter::specs_setter(basic_format_specs& specs): specs_(specs) {}
	constexpr void specs_setter::on_align(Align aln) const { specs_.alignment_ = aln; }
	constexpr void specs_setter::on_sign(Sign sgn) const { specs_.sgn_ = sgn; }
	constexpr void specs_setter::on_hash() const { specs_.alt_ = true; }
	constexpr void specs_setter::on_zero() const { specs_.leading_zero_ = true; }
	constexpr void specs_setter::on_width(int width) const { specs_.width_ = width; }
	constexpr void specs_setter::on_precision(int precision) const { specs_.precision_ = precision; }
	constexpr void specs_setter::on_localized() const { specs_.localized_ = true; }
	constexpr void specs_setter::on_type(char8_t type) const { specs_.type_ = type; }

	constexpr void specs_setter::on_fill(HStringView sv) const {
		if (sv.size() > std::size(specs_.fill_)) {
			report_error(u8"invalid fill (too long).");
		}

		const auto pos = std::copy(sv.begin(), sv.end(), specs_.fill_);
		std::fill(pos, std::end(specs_.fill_), char8_t{});
		specs_.fill_length_ = static_cast<uint8_t>(sv.size());
	}

#pragma endregion specs_setter

#pragma region dynamic_specs_handler

	constexpr dynamic_specs_handler::dynamic_specs_handler(dynamic_format_specs& specs, parse_context& parse_ctx)
		: specs_setter(specs), dynamic_specs_(specs), parse_ctx_(parse_ctx) {}

	constexpr void dynamic_specs_handler::on_dynamic_width(size_t arg_id) const {
		parse_ctx_.check_arg_id(arg_id);
		dynamic_specs_.dynamic_width_index_ = verify_dynamic_arg_index_in_range(arg_id);
	}

	constexpr void dynamic_specs_handler::on_dynamic_width(auto_id_tag) const {
		dynamic_specs_.dynamic_width_index_ = verify_dynamic_arg_index_in_range(parse_ctx_.next_arg_id());
	}

	constexpr void dynamic_specs_handler::on_dynamic_precision(size_t arg_id) const {
		parse_ctx_.check_arg_id(arg_id);
		dynamic_specs_.dynamic_precision_index_ = verify_dynamic_arg_index_in_range(arg_id);
	}

	constexpr void dynamic_specs_handler::on_dynamic_precision(auto_id_tag) const {
		dynamic_specs_.dynamic_precision_index_ = verify_dynamic_arg_index_in_range(parse_ctx_.next_arg_id());
	}

	constexpr int dynamic_specs_handler::verify_dynamic_arg_index_in_range(size_t idx) {
		if (idx > static_cast<size_t>(std::numeric_limits<int>::max())) {
			report_error(u8"dynamic width or precision index too large.");
		}

		return static_cast<int>(idx);
	}

#pragma endregion dynamic_specs_handler

#pragma region compile_parse

	// Checks that the type of the argument printed by a replacement
	// field with format specs actually satisfies the requirements for
	// that format spec. If the requirements are met then calls the base class
	// handler method.
	template<typename Handler>
	class specs_checker : public Handler {
	public:
		constexpr explicit specs_checker(const Handler& handler_inst, Type arg_type)
			: Handler(handler_inst), arg_type_(arg_type) {}

		constexpr void require_numeric_argument() const {
			if (!is_arithmetic_fmt_type(arg_type_)) {
				report_error(u8"format specifier requires numeric argument.");
			}
		}

		constexpr void check_precision() const {
			if (is_integral_fmt_type(arg_type_) || arg_type_ == Type::pointer_type) {
				report_error(u8"precision not allowed for this argument type.");
			}
		}

		constexpr void on_localized() {
			require_numeric_argument();
			Handler::on_localized();
		}

		constexpr void on_hash() {
			need_arithmetic_presentation_type_ = true;
			require_numeric_argument();
			Handler::on_hash();
		}

		constexpr void on_sign(Sign sgn) {
			need_arithmetic_presentation_type_ = true;
			require_numeric_argument();
			Handler::on_sign(sgn);
		}

		constexpr void on_zero() {
			need_arithmetic_presentation_type_ = true;
			require_numeric_argument();
			Handler::on_zero();
		}

		constexpr void on_precision(int precision) {
			check_precision();
			Handler::on_precision(precision);
		}

		template<typename T>
		constexpr void on_dynamic_precision(T value) {
			check_precision();
			Handler::on_dynamic_precision(value);
		}

		constexpr void on_type(char8_t type) {
			if (type < 0 || type > std::numeric_limits<signed char>::max()) {
				report_error(u8"invalid type specification.");
			}
			const char narrow_type = static_cast<char>(type);
			enum class Presentation_type_category {
				Default,
				Integer,
				Floating,
				String,
				Pointer,
				Char,
				Escape,
			};
			auto catagory = Presentation_type_category::Default;
			switch (narrow_type) {
				case '\0':
					break;
				case '?':
					catagory = Presentation_type_category::Escape;
					break;
				case 's':
					catagory = Presentation_type_category::String;
					break;
				case 'c':
					catagory = Presentation_type_category::Char;
					break;
				case 'd':
				case 'B':
				case 'b':
				case 'X':
				case 'x':
				case 'o':
					catagory = Presentation_type_category::Integer;
					break;
				case 'A':
				case 'a':
				case 'E':
				case 'e':
				case 'F':
				case 'f':
				case 'G':
				case 'g':
					catagory = Presentation_type_category::Floating;
					break;
				case 'p':
					catagory = Presentation_type_category::Pointer;
					break;
				default:
					report_error(u8"invalid presentation type specifier");
			}

			switch (arg_type_) {
				case Type::none_type:
					report_error(u8"invalid argument type.");
					break;
				case Type::bool_type:
					if (catagory == Presentation_type_category::Default) {
						catagory = Presentation_type_category::String;
					}
				// note, we don't get a call if there isn't a type, but none is valid for everything.
					if (catagory != Presentation_type_category::String && catagory != Presentation_type_category::Integer) {
						report_error(u8"invalid presentation type for bool");
					}
					break;
				case Type::char_type:
					if (catagory == Presentation_type_category::Default) {
						catagory = Presentation_type_category::Char;
					}

					if (catagory != Presentation_type_category::Char && catagory != Presentation_type_category::Integer && catagory != Presentation_type_category::Escape) {
						report_error(u8"invalid presentation type for char8_t");
					}
					break;
				case Type::int_type:
				case Type::uint_type:
				case Type::long_long_type:
				case Type::ulong_long_type:
					if (catagory == Presentation_type_category::Default) {
						catagory = Presentation_type_category::Integer;
					}

					if (catagory != Presentation_type_category::Integer && catagory != Presentation_type_category::Char) {
						report_error(u8"invalid presentation type for integer");
					}
					break;
				case Type::float_type:
				case Type::double_type:
				case Type::long_double_type:
					if (catagory == Presentation_type_category::Default) {
						catagory = Presentation_type_category::Floating;
					}

					if (catagory != Presentation_type_category::Floating) {
						report_error(u8"invalid presentation type for floating-point");
					}
					break;
				case Type::cstring_type:
				case Type::string_type:
					if (catagory == Presentation_type_category::Default) {
						catagory = Presentation_type_category::String;
					}

					if (catagory != Presentation_type_category::String && catagory != Presentation_type_category::Escape) {
						report_error(u8"invalid presentation type for string");
					}
					break;
				case Type::pointer_type:
					if (catagory == Presentation_type_category::Default) {
						catagory = Presentation_type_category::Pointer;
					}

					if (catagory != Presentation_type_category::Pointer) {
						report_error(u8"invalid presentation type for pointer");
					}
					break;
				case Type::custom_type:
					// there's no checking we can do here for custom types
					// (however if a custom type uses a standard formatter
					// to do its spec parsing it should get the above checks)
					break;
			}

			if (need_arithmetic_presentation_type_ && catagory
				!=
				Presentation_type_category::Integer && catagory != Presentation_type_category::Floating
			) {
				report_error(u8"modifier requires an integer presentation type for bool");
			}
			Handler::on_type(type);
		}

	private:
		Type arg_type_;
		// we'll see this if we get a modifier that requires an integer presentation type
		// for types that can have either integer or non-integer presentation types (charT or bool)
		bool need_arithmetic_presentation_type_ = false;
	};

	template<typename Callbacks>
	constexpr const char8_t* parse_align(const char8_t* first, const char8_t* last, Callbacks&& callbacks) {
		// align and fill
		assert(first != last && *first != '}');
		auto parsed_align = Align::none;

		const int units = unicode::utf8_code_units_in_next_character(first, last);
		if (units < 0) {
			// invalid fill character encoding
			report_error(u8"invalid format string.");
		}
		auto align_pt = first + units;

		if (align_pt == last) {
			align_pt = first;
		}

		for (;;) {
			switch (*align_pt) {
				case '<':
					parsed_align = Align::left;
					break;
				case '>':
					parsed_align = Align::right;
					break;
				case '^':
					parsed_align = Align::center;
					break;
				default:
					break;
			}

			if (parsed_align != Align::none) {
				if (align_pt != first) {
					if (*first == '{') {
						report_error(u8"invalid fill character '{'");
					}
					callbacks.on_fill({first, static_cast<size_t>(align_pt - first)});
					first = align_pt + 1;
				} else {
					++first;
				}
				callbacks.on_align(parsed_align);
				break;
			}
			if (align_pt == first) {
				break;
			}
			align_pt = first;
		}

		return first;
	}

	template<typename Callbacks>
	constexpr const char8_t* parse_width(const char8_t* first, const char8_t* last, Callbacks&& callbacks) {
		assert(first != last);
		if ('1' <= *first && *first <= '9') {
			int value = 0;
			first = fmt::parse_nonnegative_integer(first, last, value);
			callbacks.on_width(value);
		} else if (*first == '{') {
			++first;
			if (first != last) {
				// Adapts a type modeling _Width_adapter_callbacks to model _Parse_arg_id_callbacks.
				// Used in _Parse_width so that _Parse_arg_id can be used to parse dynamic widths.
				struct width_adapter {
					Callbacks& callbacks_;

					constexpr explicit width_adapter(Callbacks& handler) : callbacks_(handler) {}

					constexpr void on_auto_id() {
						callbacks_.on_dynamic_width(auto_id_tag{});
					}

					constexpr void on_manual_id(const size_t id) {
						callbacks_.on_dynamic_width(id);
					}
				} adapter{callbacks};

				first = fmt::parse_arg_id(first, last, adapter);
			}

			if (first == last || *first != '}') {
				report_error(u8"invalid format string.");
			}
			++first;
		}
		return first;
	}

	template<typename Callbacks>
	constexpr const char8_t* parse_precision(const char8_t* first, const char8_t* last, Callbacks&& callbacks) {
		++first;
		char8_t ch = '\0';
		if (first != last) {
			ch = *first;
		}

		if ('0' <= ch && ch <= '9') {
			int precision = 0;
			first = parse_nonnegative_integer(first, last, precision);
			callbacks.on_precision(precision);
		} else if (ch == '{') {
			++first;
			if (first != last) {
				// Adapts a type modeling precision_adapter_callbacks to model parse_arg_id_callbacks.
				// Used in parse_precision so that parse_arg_id can be used to parse dynamic precisions.
				struct precision_adapter {
					Callbacks& callbacks_;

					constexpr explicit precision_adapter(Callbacks& handler) : callbacks_(handler) {}

					constexpr void on_auto_id() {
						callbacks_.on_dynamic_precision(auto_id_tag{});
					}

					constexpr void on_manual_id(const size_t id) {
						callbacks_.on_dynamic_precision(id);
					}
				} adapter{callbacks};

				first = fmt::parse_arg_id(first, last, adapter);
			}

			if (first == last || *first != '}') {
				report_error(u8"invalid format string.");
			}
			++first;
		} else {
			report_error(u8"missing precision specifier.");
		}

		return first;
	}

	template<typename Callbacks>
	constexpr const char8_t* parse_format_specs(const char8_t* first, const char8_t* last, Callbacks&& callbacks) {
		if (first == last || *first == '}') {
			return first;
		}

		first = fmt::parse_align(first, last, callbacks);
		if (first == last) {
			return first;
		}

		switch (*first) {
			case '+':
				callbacks.on_sign(Sign::plus);
				++first;
				break;
			case '-':
				callbacks.on_sign(Sign::minus);
				++first;
				break;
			case ' ':
				callbacks.on_sign(Sign::space);
				++first;
				break;
			default:
				break;
		}

		if (first == last) {
			return first;
		}

		if (*first == '#') {
			callbacks.on_hash();
			if (++first == last) {
				return first;
			}
		}

		if (*first == '0') {
			callbacks.on_zero();
			if (++first == last) {
				return first;
			}
		}

		first = fmt::parse_width(first, last, callbacks);
		if (first == last) {
			return first;
		}

		if (*first == '.') {
			first = fmt::parse_precision(first, last, callbacks);
			if (first == last) {
				return first;
			}
		}

		if (*first == 'L') {
			callbacks.on_localized();
			if (++first == last) {
				return first;
			}
		}

		// If there's anything remaining we assume it's a type.
		if (*first != '}') {
			callbacks.on_type(*first);
			++first;
		} else {
			// call the type callback so it gets a default type, this is required
			// since _Specs_checker needs to be able to tell that it got a default type
			// to raise an error for default formatted bools with a sign modifier
			callbacks.on_type(u8'\0');
		}

		return first;
	}

	template<typename T, Type ArgType>
	constexpr auto formatter_base<T, ArgType>::parse(parse_context& parse_ctx) {
		specs_checker handler(dynamic_specs_handler{specs_, parse_ctx}, ArgType);
		const auto iter = fmt::parse_format_specs(parse_ctx.unchecked_begin(), parse_ctx.unchecked_end(), handler);
		if (iter != parse_ctx.unchecked_end() && *iter != '}') {
			report_error(u8"missing '}' in format string.");
		}
		return parse_ctx.begin() + (iter - parse_ctx.unchecked_begin());
	}

#pragma endregion compile_parse
}

namespace hana::fmt
{
#pragma region format_arg

	template<typename Visitor>
	constexpr decltype(auto) format_arg::visit(Visitor&& vis) {
		using enum Type;
		switch (active_state_) {
			case none_type:
				return std::forward<Visitor>(vis)(value_.no_state_);
			case int_type:
				return std::forward<Visitor>(vis)(value_.int_state_);
			case uint_type:
				return std::forward<Visitor>(vis)(value_.uint_state_);
			case long_long_type:
				return std::forward<Visitor>(vis)(value_.long_long_state_);
			case ulong_long_type:
				return std::forward<Visitor>(vis)(value_.ulong_long_state_);
			case bool_type:
				return std::forward<Visitor>(vis)(value_.bool_state_);
			case char_type:
				return std::forward<Visitor>(vis)(value_.char_state_);
			case float_type:
				return std::forward<Visitor>(vis)(value_.float_state_);
			case double_type:
				return std::forward<Visitor>(vis)(value_.double_state_);
			case long_double_type:
				return std::forward<Visitor>(vis)(value_.long_double_state_);
			case pointer_type:
				return std::forward<Visitor>(vis)(value_.pointer_state_);
			case cstring_type:
				return std::forward<Visitor>(vis)(value_.cstring_state_);
			case string_type:
				return std::forward<Visitor>(vis)(value_.string_state_);
			case custom_type:
				return std::forward<Visitor>(vis)(value_.custom_state_);
			default:
				abort();
		}
	}

	constexpr format_arg::operator bool() const noexcept {
		return active_state_ != fmt::Type::none_type;
	}

	constexpr format_arg::format_arg() noexcept: active_state_(fmt::Type::none_type) {}
	constexpr format_arg::format_arg(int val) noexcept: value_(val), active_state_(fmt::Type::int_type) {}
	constexpr format_arg::format_arg(unsigned int val) noexcept: value_(val), active_state_(fmt::Type::uint_type) {}
	constexpr format_arg::format_arg(long long val) noexcept: value_(val), active_state_(fmt::Type::long_long_type) {}
	constexpr format_arg::format_arg(unsigned long long val) noexcept: value_(val), active_state_(fmt::Type::ulong_long_type) {}
	constexpr format_arg::format_arg(bool val) noexcept : value_(val), active_state_(fmt::Type::bool_type) {}
	constexpr format_arg::format_arg(char8_t val) noexcept : value_(val), active_state_(fmt::Type::char_type) {}
	constexpr format_arg::format_arg(float val) noexcept: value_(val), active_state_(fmt::Type::float_type) {}
	constexpr format_arg::format_arg(double val) noexcept : value_(val), active_state_(fmt::Type::double_type) {}
	constexpr format_arg::format_arg(long double val) noexcept: value_(val), active_state_(fmt::Type::long_double_type) {}
	constexpr format_arg::format_arg(const void* val) noexcept: value_(val), active_state_(fmt::Type::pointer_type) {}
	constexpr format_arg::format_arg(const char8_t* val) noexcept: value_(val), active_state_(fmt::Type::cstring_type) {}
	constexpr format_arg::format_arg(HStringView val) noexcept: value_(val), active_state_(fmt::Type::string_type) {}
	template<typename T> constexpr format_arg::format_arg(const T& val) noexcept: value_(val), active_state_(fmt::Type::custom_type) {}


#pragma endregion format_arg

#pragma region format_args

	constexpr format_args::format_args() noexcept: desc_(0), args_(nullptr) {}

	constexpr format_args::format_args(fmt::format_arg_store<0>, size_t) noexcept : format_args() {}

	template<size_t NUM_ARGS>
	format_args::format_args(const fmt::format_arg_store<NUM_ARGS>& s, size_t desc)
		: desc_(desc), values_(s.args) {}

#pragma endregion format_args

#pragma region parse_context

	constexpr parse_context::parse_context(HStringView fmt, size_t num_args) noexcept
		: format_string_(fmt), num_args_(num_args) {}

	constexpr parse_context::const_iterator parse_context::begin() const noexcept {
		return format_string_.begin();
	}

	constexpr parse_context::const_iterator parse_context::end() const noexcept {
		return format_string_.end();
	}

	constexpr const char8_t* parse_context::unchecked_begin() const noexcept {
		return format_string_.data();
	}

	constexpr const char8_t* parse_context::unchecked_end() const noexcept {
		return format_string_.data() + format_string_.size();
	}

	constexpr void parse_context::advance_to(const const_iterator& iter) {
		assert(format_string_.begin() <= iter && iter <= format_string_.end());

		const auto diff = static_cast<size_t>(iter - format_string_.begin());
		format_string_.remove_prefix(diff);
	}

	constexpr size_t parse_context::next_arg_id() {
		if (next_arg_id_ < 0) {
			fmt::report_error(u8"can not switch from manual to automatic indexing");
		}

		if (std::is_constant_evaluated()) {
			if (next_arg_id_ >= num_args_) fmt::report_error(u8"argument not found");
		}

		return static_cast<size_t>(next_arg_id_++);
	}

	constexpr void parse_context::check_arg_id(size_t id) {
		if (std::is_constant_evaluated()) {
			if (id >= num_args_) fmt::report_error(u8"argument not found");
		}

		if (next_arg_id_ > 0) {
			fmt::report_error(u8"can not switch from automatic to manual indexing");
		}
		next_arg_id_ = -1;
	}

#pragma endregion parse_context

#pragma region context

	inline context::context(iterator iter, format_args ctx_args, const std::locale* loc)
		: out_(iter), args_(ctx_args), loc_(loc) {}

	inline format_arg context::arg(size_t id) const noexcept {
		return args_.get(id);
	}

	inline context::iterator context::out() const {
		return out_;
	}

	inline void context::advance_to(iterator it) {
		out_ = it;
	}

	inline const format_args& context::get_args() const noexcept {
		return args_;
	}

	inline const std::locale* context::get_lazy_locale() const {
		return loc_;
	}

#pragma endregion context

	template<typename... Args>
	constexpr auto make_format_store(Args&... args) noexcept {
		static_assert(
			(formattable<std::remove_cvref_t<Args>> && ...),
			"Cannot format an argument. To make type T formattable, provide a formatter<T> specialization. "
			"See N4950 [format.arg.store]/2 and [formatter.requirements]."
		);
		return fmt::format_arg_store<sizeof...(Args)>{args...};
	}
}

namespace hana
{
	//=====================> format_to <========================

	template<std::output_iterator<const char8_t&> OutputIt, typename... T>
	OutputIt format_to(OutputIt out, fmt::format_string<T...> fmt, T&&... args) {
		constexpr auto DESC = fmt::make_descriptor<T...>();
		return hana::vformat_to(out, fmt.get(), fmt::format_args(fmt::make_format_store(args...), DESC));
	}

	template<std::output_iterator<const char8_t&> OutputIt>
	OutputIt vformat_to(OutputIt out, HStringView fmt, fmt::format_args args) {
		auto buf = fmt::iterator_buffer<OutputIt, fmt::buffer_traits>(out);
		fmt::vformat_to(buf, fmt, args);
		return buf.out();
	}

	inline char8_t* vformat_to(char8_t* out, HStringView fmt, fmt::format_args args) {
		return fmt::vformat_to(out, fmt, args);
	}

	template<std::output_iterator<const char8_t&> OutputIt, typename... T>
	OutputIt format_to(OutputIt out, const std::locale& loc, fmt::format_string<T...> fmt, T&&... args) {
		constexpr auto DESC = fmt::make_descriptor<T...>();
		return hana::vformat_to(out, loc, fmt.get(), fmt::format_args(fmt::make_format_store(args...), DESC));
	}

	template<std::output_iterator<const char8_t&> OutputIt>
	OutputIt vformat_to(OutputIt out, const std::locale& loc, HStringView fmt, fmt::format_args args) {
		auto buf = fmt::iterator_buffer<OutputIt, fmt::buffer_traits>(out);
		fmt::vformat_to(buf, fmt, args, &loc);
		return buf.out();
	}

	inline char8_t* vformat_to(char8_t* out, const std::locale& loc, HStringView fmt, fmt::format_args args) {
		return fmt::vformat_to(out, fmt, args, &loc);
	}

	//====================> format_to_n <=======================

	template<std::output_iterator<const char8_t&> OutputIt, typename... T>
	fmt::format_to_n_result<OutputIt> format_to_n(OutputIt out, size_t n, fmt::format_string<T...> fmt, T&&... args) {
		constexpr auto DESC = fmt::make_descriptor<T...>();
		return hana::vformat_to_n(out, n, fmt.get(), fmt::format_args(fmt::make_format_store(args...), DESC));
	}

	template<std::output_iterator<const char8_t&> OutputIt>
	fmt::format_to_n_result<OutputIt> vformat_to_n(OutputIt out, size_t n, HStringView fmt, fmt::format_args args) {
		auto buf = fmt::iterator_buffer<OutputIt, fmt::fixed_buffer_traits>(out, n);
		fmt::vformat_to(buf, fmt, args);
		return {buf.out(), buf.count()};
	}

	inline fmt::format_to_n_result<char8_t*> vformat_to_n(char8_t* out, size_t n, HStringView fmt, fmt::format_args args) {
		return fmt::vformat_to_n(out, n, fmt, args);
	}

	template<std::output_iterator<const char8_t&> OutputIt, typename... T>
	fmt::format_to_n_result<OutputIt> format_to_n(OutputIt out, size_t n, const std::locale& loc, fmt::format_string<T...> fmt, T&&... args) {
		constexpr auto DESC = fmt::make_descriptor<T...>();
		return hana::vformat_to_n(out, n, loc, fmt.get(), fmt::format_args(fmt::make_format_store(args...), DESC));
	}

	template<std::output_iterator<const char8_t&> OutputIt>
	fmt::format_to_n_result<OutputIt> vformat_to_n(OutputIt out, size_t n, const std::locale& loc, HStringView fmt, fmt::format_args args) {
		auto buf = fmt::iterator_buffer<OutputIt, fmt::fixed_buffer_traits>(out, n);
		fmt::vformat_to(buf, fmt, args, &loc);
		return {buf.out(), buf.count()};
	}

	inline fmt::format_to_n_result<char8_t*> vformat_to_n(char8_t* out, size_t n, const std::locale& loc, HStringView fmt, fmt::format_args args) {
		return fmt::vformat_to_n(out, n, fmt, args, &loc);
	}

	//===============> format_to fixed array <==================

	template<size_t N, typename... T>
	fmt::format_to_result format_to(char8_t (&out)[N], fmt::format_string<T...> fmt, T&&... args) {
		constexpr auto DESC = fmt::make_descriptor<T...>();
		return hana::vformat_to(out, fmt.get(), fmt::format_args(fmt::make_format_store(args...), DESC));
	}

	template<size_t N>
	fmt::format_to_result vformat_to(char8_t (&out)[N], HStringView fmt, fmt::format_args args) {
		auto result = hana::vformat_to_n(out, N, fmt, args);
		return {result.out, result.size > N};
	}

	template<size_t N, typename... T>
	fmt::format_to_result format_to(char8_t (&out)[N], const std::locale& loc, fmt::format_string<T...> fmt, T&&... args) {
		constexpr auto DESC = fmt::make_descriptor<T...>();
		return hana::vformat_to(out, loc, fmt.get(), fmt::format_args(fmt::make_format_store(args...), DESC));
	}

	template<size_t N>
	fmt::format_to_result vformat_to(char8_t (&out)[N], const std::locale& loc, HStringView fmt, fmt::format_args args) {
		auto result = hana::vformat_to_n(out, N, fmt, args, &loc);
		return {result.out, result.size > N};
	}

	//===================> formatted_size <=====================

	template<typename... T>
	size_t formatted_size(fmt::format_string<T...> fmt, T&&... args) {
		constexpr auto DESC = fmt::make_descriptor<T...>();
		return hana::vformatted_size(fmt.get(), fmt::format_args(fmt::make_format_store(args...), DESC));
	}

	inline size_t vformatted_size(HStringView fmt, fmt::format_args args) {
		return fmt::vformatted_size(fmt, args);
	}

	template<typename... T>
	size_t formatted_size(const std::locale& loc, fmt::format_string<T...> fmt, T&&... args) {
		constexpr auto DESC = fmt::make_descriptor<T...>();
		return hana::vformatted_size(loc, fmt.get(), fmt::format_args(fmt::make_format_store(args...), DESC));
	}

	inline size_t vformatted_size(const std::locale& loc, HStringView fmt, fmt::format_args args) {
		return fmt::vformatted_size(fmt, args, &loc);
	}

	//=======================> format <=========================

	inline HString vformat(HStringView fmt, fmt::format_args args) {
		return fmt::vformat(fmt, args);
	}

	template<typename... T>
	HString format(fmt::format_string<T...> fmt, T&&... args) {
		constexpr auto DESC = fmt::make_descriptor<T...>();
		return hana::vformat(fmt.get(), fmt::format_args(fmt::make_format_store(args...), DESC));
	}

	inline HString vformat(const std::locale& loc, HStringView fmt, fmt::format_args args) {
		return fmt::vformat(fmt, args, &loc);
	}

	template<typename... T>
	HString format(const std::locale& loc, fmt::format_string<T...> fmt, T&&... args) {
		constexpr auto DESC = fmt::make_descriptor<T...>();
		return hana::vformat(loc, fmt.get(), fmt::format_args(fmt::make_format_store(args...), DESC));
	}
}
