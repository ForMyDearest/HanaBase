#pragma once

#include "hana/platform/macros.h"
#include "hana/container/string_view.hpp"

namespace hana
{
	//! @note Strictly prohibit empty assignment
	class HString {
	public:
		//==================> aligns <==================

		using value_type = char8_t;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;

		using raw_value_type = char;
		using raw_pointer = raw_value_type*;
		using raw_const_pointer = const raw_value_type*;
		using raw_reference = raw_value_type&;
		using raw_const_reference = const raw_value_type&;

		using size_type = size_t;
		using difference_type = ptrdiff_t;

		using traits_type = std::char_traits<char8_t>;
		using allocator_type = std::allocator<char8_t>;

		using data_reference = container::VectorDataRef<value_type, false>;
		using const_data_reference = container::VectorDataRef<value_type, true>;
		using cursor = unicode::UTF8Cursor<false>;
		using const_cursor = unicode::UTF8Cursor<true>;
		using iterator = unicode::UTF8Iter<false>;
		using const_iterator = unicode::UTF8Iter<true>;
		using reverse_iterator = unicode::UTF8IterInv<false>;
		using const_reverse_iterator = unicode::UTF8IterInv<true>;
		using range_t = unicode::UTF8Range<false>;
		using const_range = unicode::UTF8Range<true>;
		using reverse_range = unicode::UTF8RangeInv<false>;
		using const_reverse_range = unicode::UTF8RangeInv<true>;

		static constexpr size_type SSOSize = 31;
		static constexpr size_type SSOBufferSize = SSOSize + 1;
		static constexpr size_type SSOCapacity = SSOSize - 1;
		static constexpr size_type npos = HStringView::npos;

		static_assert(SSOBufferSize % 4 == 0, "SSOSize must be 4n - 1");
		static_assert(SSOBufferSize > sizeof(size_type) * 2 + sizeof(pointer), "SSOSize must be larger than heap data size");
		static_assert(SSOBufferSize < 128, "SSOBufferSize must be less than 127"); // sso_size_ max

		//==================> join <==================

		template<typename... Args> static HString concat(Args&&... string_or_view);
		template<typename Container> static HString join(const Container& container, HStringView separator, bool skip_empty = true, HStringView trim_chs = {});

		//==================> ctor & dtor <==================

		HANA_BASE_API HString();
		HANA_BASE_API HString(size_type count, value_type ch);
		HString(const HString& other);
		HANA_BASE_API HString(HString&& rhs) noexcept;

		HString(const char* str);
		HString(const char8_t* str);
		HString(const char16_t* str);
		HString(const char32_t* str);
		HString(const wchar_t* str);

		HString(const char* str, size_type count);
		HString(const char8_t* str, size_type count);
		HANA_BASE_API HString(const char16_t* str, size_type count);
		HANA_BASE_API HString(const char32_t* str, size_type count);
		HString(const wchar_t* str, size_type count);

		HANA_BASE_API HString(HStringView view);
		HString(HStringView view, size_type pos);
		HString(HStringView view, size_type pos, size_type count);

		HANA_BASE_API ~HString() noexcept;

		//==================> assign <==================

		HANA_BASE_API HString& assign(HString&& rhs) noexcept;
		HANA_BASE_API HString& assign(HStringView view);
		HString& assign(HStringView view, size_type pos, size_type count = npos);

		HString& assign(const char* str);
		HString& assign(const char8_t* str);
		HString& assign(const char16_t* str);
		HString& assign(const char32_t* str);
		HString& assign(const wchar_t* str);

		HString& assign(const char* str, size_type count);
		HString& assign(const char8_t* str, size_type count);
		HANA_BASE_API HString& assign(const char16_t* str, size_type count);
		HANA_BASE_API HString& assign(const char32_t* str, size_type count);
		HString& assign(const wchar_t* str, size_type count);

		HString& operator=(const HString& rhs);
		HString& operator=(HString&& rhs) noexcept;
		HString& operator=(HStringView view);
		HString& operator=(const char* str);
		HString& operator=(const char8_t* str);
		HString& operator=(const char16_t* str);
		HString& operator=(const char32_t* str);
		HString& operator=(const wchar_t* str);

		//==================> compare <==================

		bool operator==(const char* str) const noexcept;
		bool operator==(const char8_t* str) const noexcept;
		bool operator==(HStringView str) const noexcept;
		bool operator==(const HString& str) const noexcept;
		std::strong_ordering operator<=>(const char* str) const noexcept;
		std::strong_ordering operator<=>(const char8_t* str) const noexcept;
		std::strong_ordering operator<=>(HStringView str) const noexcept;
		std::strong_ordering operator<=>(const HString& str) const noexcept;

		//==================> iterator <==================

		pointer begin() noexcept;
		const_pointer begin() const noexcept;
		const_pointer cbegin() const noexcept;

		pointer end() noexcept;
		const_pointer end() const noexcept;
		const_pointer cend() const noexcept;

		std::reverse_iterator<pointer> rbegin() noexcept;
		std::reverse_iterator<const_pointer> rbegin() const noexcept;
		std::reverse_iterator<const_pointer> crbegin() const noexcept;

		std::reverse_iterator<pointer> rend() noexcept;
		std::reverse_iterator<const_pointer> rend() const noexcept;
		std::reverse_iterator<const_pointer> crend() const noexcept;

		cursor cursor_begin();
		cursor cursor_end();
		iterator iter();
		reverse_iterator iter_inv();
		range_t range();
		reverse_range range_inv();

		const_cursor cursor_begin() const;
		const_cursor cursor_end() const;
		const_iterator iter() const;
		const_reverse_iterator iter_inv() const;
		const_range range() const;
		const_reverse_range range_inv() const;

		//==================> size <==================

		bool empty() const noexcept;
		size_type length() const noexcept;
		size_type text_length() const noexcept;
		size_type capacity() const noexcept;
		size_type slack() const noexcept;
		size_type size() const noexcept;
		size_type max_size() const noexcept;
		template<is_char_v Char> size_type to_size() const noexcept;

		//==================> data access <==================

		reference at(size_type pos);
		reference operator[](size_type pos);
		reference front();
		reference back();
		pointer data() noexcept;

		const_reference at(size_type pos) const;
		const_reference operator[](size_type pos) const;
		const_reference front() const;
		const_reference back() const;
		const_pointer data() const noexcept;

		raw_reference raw_at(size_type pos);
		raw_reference raw_front();
		raw_reference raw_back();
		raw_pointer raw_data() noexcept;

		raw_const_reference raw_at(size_type pos) const;
		raw_const_reference raw_front() const;
		raw_const_reference raw_back() const;
		raw_const_pointer raw_data() const noexcept;

		const_pointer c_str() const noexcept;
		unicode::UTF8Seq at_text(size_type index) const;
		unicode::UTF8Seq last_text(size_type index) const;
		bool is_sso() const noexcept;
		bool is_heap() const noexcept;
		bool is_valid_index(size_type index) const noexcept;
		size_type buffer_index_to_text(size_type index) const noexcept;
		size_type text_index_to_buffer(size_type index) const noexcept;

		//==================> sub string & view <==================

		operator HStringView() const noexcept;
		HStringView first_view(size_type count) const;
		HStringView last_view(size_type count) const;
		HStringView subview(size_type start, size_type count = npos) const noexcept;
		HString first_str(size_type count) const;
		HString last_str(size_type count) const;
		HString substr(size_type pos = 0, size_type count = npos) const;

		//==================> add <==================

		HANA_BASE_API HString& insert(size_type index, size_type count, value_type ch);
		HString& insert(size_type index, unicode::UTF8Seq seq);
		HANA_BASE_API HString& insert(size_type index, HStringView view);
		HANA_BASE_API HString& insert(size_type index, const char16_t* str);
		HANA_BASE_API HString& insert(size_type index, const char32_t* str);
		HString& insert(size_type index, const wchar_t* str);

		HANA_BASE_API HString& append(size_type count, value_type ch);
		HString& append(unicode::UTF8Seq seq);
		HANA_BASE_API HString& append(HStringView view);
		HANA_BASE_API HString& append(const char16_t* str);
		HANA_BASE_API HString& append(const char32_t* str);
		HString& append(const wchar_t* str);

		HString& push_back(value_type ch);

		void operator+=(value_type ch);
		void operator+=(unicode::UTF8Seq seq);
		void operator+=(HStringView view);
		void operator+=(const char16_t* str);
		void operator+=(const char32_t* str);
		void operator+=(const wchar_t* str);

		//==================> remove <==================

		HANA_BASE_API HString& clear() noexcept;
		HANA_BASE_API HString& pop_back(size_type count = 1);
		HANA_BASE_API HString& erase(size_type index = 0, size_type count = npos);

		//==================> replace <==================

		HANA_BASE_API HString& replace(size_type pos, size_type count, const_pointer cstr, size_type count2);
		HString& replace(size_type pos, size_type count, HStringView view);

		HString Replace(size_type pos, size_type count, const_pointer cstr, size_type count2) const;
		HString Replace(size_type pos, size_type count, HStringView view) const;

		//==================> starts & ends with <==================

		bool starts_with(HStringView sv) const noexcept;
		bool starts_with(value_type ch) const noexcept;
		bool starts_with(unicode::UTF8Seq seq) const;

		bool ends_with(HStringView sv) const noexcept;
		bool ends_with(value_type ch) const noexcept;
		bool ends_with(unicode::UTF8Seq seq) const;

		//==================> contains & count <==================

		bool contains(HStringView sv) const noexcept;
		bool contains(value_type ch) const noexcept;
		bool contains(unicode::UTF8Seq seq) const;

		size_type count(HStringView pattern) const;
		size_type count(unicode::UTF8Seq seq) const;

		//==================> find <==================

		data_reference find(HStringView v, size_type pos = 0) noexcept;
		data_reference find(value_type ch, size_type pos = 0) noexcept;
		data_reference find(unicode::UTF8Seq seq, size_type pos = 0);
		data_reference find(const_pointer s, size_type pos, size_type count);

		data_reference find_first_of(HStringView v, size_type pos = 0) noexcept;
		data_reference find_first_of(value_type ch, size_type pos = 0) noexcept;
		data_reference find_first_of(unicode::UTF8Seq seq, size_type pos = 0);
		data_reference find_first_of(const_pointer s, size_type pos, size_type count);

		data_reference find_first_not_of(HStringView v, size_type pos = 0) noexcept;
		data_reference find_first_not_of(value_type ch, size_type pos = 0) noexcept;
		data_reference find_first_not_of(unicode::UTF8Seq seq, size_type pos = 0);
		data_reference find_first_not_of(const_pointer s, size_type pos, size_type count);

		data_reference rfind(HStringView v, size_type pos = npos) noexcept;
		data_reference rfind(value_type ch, size_type pos = npos) noexcept;
		data_reference rfind(unicode::UTF8Seq seq, size_type pos = npos);
		data_reference rfind(const_pointer s, size_type pos, size_type count);

		data_reference find_last_of(HStringView v, size_type pos = npos) noexcept;
		data_reference find_last_of(value_type ch, size_type pos = npos) noexcept;
		data_reference find_last_of(unicode::UTF8Seq seq, size_type pos = npos);
		data_reference find_last_of(const_pointer s, size_type pos, size_type count);

		data_reference find_last_not_of(HStringView v, size_type pos = npos) noexcept;
		data_reference find_last_not_of(value_type ch, size_type pos = npos) noexcept;
		data_reference find_last_not_of(unicode::UTF8Seq seq, size_type pos = npos);
		data_reference find_last_not_of(const_pointer s, size_type pos, size_type count);

		const_data_reference find(HStringView v, size_type pos = 0) const noexcept;
		const_data_reference find(value_type ch, size_type pos = 0) const noexcept;
		const_data_reference find(unicode::UTF8Seq seq, size_type pos = 0) const;
		const_data_reference find(const_pointer s, size_type pos, size_type count) const;

		const_data_reference find_first_of(HStringView v, size_type pos = 0) const noexcept;
		const_data_reference find_first_of(value_type ch, size_type pos = 0) const noexcept;
		const_data_reference find_first_of(unicode::UTF8Seq seq, size_type pos = 0) const;
		const_data_reference find_first_of(const_pointer s, size_type pos, size_type count) const;

		const_data_reference find_first_not_of(HStringView v, size_type pos = 0) const noexcept;
		const_data_reference find_first_not_of(value_type ch, size_type pos = 0) const noexcept;
		const_data_reference find_first_not_of(unicode::UTF8Seq seq, size_type pos = 0) const;
		const_data_reference find_first_not_of(const_pointer, size_type pos, size_type count) const;

		const_data_reference rfind(HStringView v, size_type pos = npos) const noexcept;
		const_data_reference rfind(value_type ch, size_type pos = npos) const noexcept;
		const_data_reference rfind(unicode::UTF8Seq seq, size_type pos = npos) const;
		const_data_reference rfind(const_pointer, size_type pos, size_type count) const;

		const_data_reference find_last_of(HStringView v, size_type pos = npos) const noexcept;
		const_data_reference find_last_of(value_type ch, size_type pos = npos) const noexcept;
		const_data_reference find_last_of(unicode::UTF8Seq seq, size_type pos = npos) const;
		const_data_reference find_last_of(const_pointer s, size_type pos, size_type count) const;

		const_data_reference find_last_not_of(HStringView v, size_type pos = npos) const noexcept;
		const_data_reference find_last_not_of(value_type ch, size_type pos = npos) const noexcept;
		const_data_reference find_last_not_of(unicode::UTF8Seq seq, size_type pos = npos) const;
		const_data_reference find_last_not_of(const_pointer s, size_type pos, size_type count) const;

		//==================> remove prefix & prefix <==================

		HString& remove_prefix(size_type n);
		HString& remove_prefix(HStringView prefix);
		HString& remove_prefix(unicode::UTF8Seq prefix);

		HString& remove_suffix(size_type n);
		HString& remove_suffix(HStringView suffix);
		HString& remove_suffix(unicode::UTF8Seq suffix);

		HString RemovePrefix(size_type n) const;
		HString RemovePrefix(HStringView prefix) const;
		HString RemovePrefix(unicode::UTF8Seq prefix) const;

		HString RemoveSuffix(size_type n) const;
		HString RemoveSuffix(HStringView suffix) const;
		HString RemoveSuffix(unicode::UTF8Seq suffix) const;

		//==================> partition <==================

		std::array<HStringView, 3> partition(HStringView delimiter) const;
		std::array<HStringView, 3> partition(unicode::UTF8Seq delimiter) const;

		//==================> trim <==================

		HString& trim(HStringView characters = u8" \t");
		HString& trim_start(HStringView characters = u8" \t");
		HString& trim_end(HStringView characters = u8" \t");
		HString& trim(unicode::UTF8Seq seq);
		HString& trim_start(unicode::UTF8Seq seq);
		HString& trim_end(unicode::UTF8Seq seq);
		HString& trim_invalid();
		HString& trim_invalid_start();
		HString& trim_invalid_end();

		HString Trim(HStringView characters = u8" \t") const;
		HString TrimStart(HStringView characters = u8" \t") const;
		HString TrimEnd(HStringView characters = u8" \t") const;
		HString Trim(unicode::UTF8Seq seq) const;
		HString TrimStart(unicode::UTF8Seq seq) const;
		HString TrimEnd(unicode::UTF8Seq seq) const;
		HString TrimInvalid() const;
		HString TrimInvalidStart() const;
		HString TrimInvalidEnd() const;

		//==================> split <==================

		template<internal::CanAdd<HStringView> Buffer>
		size_type split(Buffer& out, HStringView delimiter, bool cull_empty = false, size_type limit = npos) const;

		template<std::invocable<HStringView> F>
		size_type split_each(F&& func, HStringView delimiter, bool cull_empty = false, size_type limit = npos) const;

		template<internal::CanAdd<HStringView> Buffer>
		size_type split(Buffer& out, unicode::UTF8Seq delimiter, bool cull_empty = false, size_type limit = npos) const;

		template<std::invocable<HStringView> F>
		size_type split_each(F&& func, unicode::UTF8Seq delimiter, bool cull_empty = false, size_type limit = npos) const;

		//==================> misc <==================

		HANA_BASE_API void reserve(size_type new_cap);
		void release(size_type reserve_capacity = 0);
		HANA_BASE_API void resize(size_type count);
		HANA_BASE_API void resize(size_type count, value_type ch);
		void swap(HString& other) noexcept;
		HString& reverse(size_type start = 0, size_type count = npos);
		size_type copy(pointer dest, size_type count, size_type pos = 0) const;

	private:
		friend struct StringHelper;

		union {
			struct {
				pointer data_;
				size_type size_;
				size_type capacity_;
			};

			struct {
				value_type sso_data_[SSOSize];
				uint8_t sso_flag_: 1;
				uint8_t sso_size_: 7;
			};

			uint8_t buffer_[SSOBufferSize];
		};
	};
}

namespace hana::internal
{
	using const_wchar_ptr = std::conditional_t<sizeof(wchar_t) == sizeof(char16_t), const char16_t*, const char32_t*>;
}

// join
namespace hana
{
	template<typename... Args>
	HString HString::concat(Args&&... string_or_view) {
		std::array<HStringView, sizeof...(Args)> args{HStringView{string_or_view}...};

		// calc size
		size_type total_size = 0;
		for (const auto& arg: args) {
			total_size += arg.size();
		}

		// combine
		HString result;
		result.reserve(total_size);
		for (const auto& arg: args) {
			result.append(arg);
		}

		return result;
	}

	template<typename Container>
	HString HString::join(const Container& container, HStringView separator, bool skip_empty, HStringView trim_chs) {
		// calc size
		size_type total_size = 0;
		bool is_first_append = false;
		for (const auto& str: container) {
			HStringView view{str};

			// trim
			if (!trim_chs.empty()) {
				view = view.trim(trim_chs);
			}

			// skip empty
			if (skip_empty && view.empty()) continue;

			// append separator
			if (!is_first_append) {
				is_first_append = true;
			} else {
				total_size += separator.size();
			}

			// append item
			total_size += view.size();
		}

		// combine
		HString result;
		result.reserve(total_size);
		is_first_append = false;
		for (const auto& str: container) {
			HStringView view{str};

			// trim
			if (!trim_chs.empty()) {
				view = view.trim(trim_chs);
			}

			// skip empty
			if (skip_empty && view.empty()) continue;

			// append separator
			if (!is_first_append) {
				is_first_append = true;
			} else {
				result.append(separator);
			}

			// append item
			result.append(view);
		}

		assert(result.size() == total_size && "Join failed");

		return result;
	}
}

// ctor & dtor
namespace hana
{
	inline HString::HString(const HString& other): HString(other.data(), other.size()) {}

	inline HString::HString(const char* str): HString(HStringView{str}) {}
	inline HString::HString(const char8_t* str): HString(HStringView{str}) {}
	inline HString::HString(const char16_t* str): HString(str, std::char_traits<char16_t>::length(str)) {}
	inline HString::HString(const char32_t* str): HString(str, std::char_traits<char32_t>::length(str)) {}
	inline HString::HString(const wchar_t* str): HString(str, std::char_traits<wchar_t>::length(str)) {}

	inline HString::HString(const char* str, size_type count): HString(HStringView{str, count}) {}
	inline HString::HString(const char8_t* str, size_type count): HString(HStringView{str, count}) {}
	inline HString::HString(const wchar_t* str, size_type count): HString(reinterpret_cast<internal::const_wchar_ptr>(str), count) {}

	inline HString::HString(HStringView view, size_type pos): HString(view.data() + pos, view.size() - pos) {}
	inline HString::HString(HStringView view, size_type pos, size_type count): HString(view.data() + pos, count) {}
}

// assign
namespace hana
{
	inline HString& HString::assign(HStringView view, size_type pos, size_type count) { return assign(view.subview(pos, count)); }

	inline HString& HString::assign(const char* str) { return assign(HStringView{str}); }
	inline HString& HString::assign(const char8_t* str) { return assign(HStringView{str}); }
	inline HString& HString::assign(const char16_t* str) { return assign(str, std::char_traits<char16_t>::length(str)); }
	inline HString& HString::assign(const char32_t* str) { return assign(str, std::char_traits<char32_t>::length(str)); }
	inline HString& HString::assign(const wchar_t* str) { return assign(str, std::char_traits<wchar_t>::length(str)); }

	inline HString& HString::assign(const char* str, size_type count) { return assign(HStringView{str, count}); }
	inline HString& HString::assign(const char8_t* str, size_type count) { return assign(HStringView{str, count}); }
	inline HString& HString::assign(const wchar_t* str, size_type count) { return assign(reinterpret_cast<internal::const_wchar_ptr>(str), count); }

	inline HString& HString::operator=(const HString& rhs) { return assign(rhs); }
	inline HString& HString::operator=(HString&& rhs) noexcept { return assign(std::move(rhs)); }
	inline HString& HString::operator=(HStringView view) { return assign(view); }
	inline HString& HString::operator=(const char* str) { return assign(str); }
	inline HString& HString::operator=(const char8_t* str) { return assign(str); }
	inline HString& HString::operator=(const char16_t* str) { return assign(str); }
	inline HString& HString::operator=(const char32_t* str) { return assign(str); }
	inline HString& HString::operator=(const wchar_t* str) { return assign(str); }
}

// compare
namespace hana
{
	inline bool HString::operator==(const char* str) const noexcept { return HStringView(*this) == str; }
	inline bool HString::operator==(const char8_t* str) const noexcept { return HStringView(*this) == str; }
	inline bool HString::operator==(HStringView str) const noexcept { return HStringView(*this) == str; }
	inline bool HString::operator==(const HString& str) const noexcept { return HStringView(*this) == HStringView(str); }
	inline std::strong_ordering HString::operator<=>(const char* str) const noexcept { return HStringView(*this) <=> str; }
	inline std::strong_ordering HString::operator<=>(const char8_t* str) const noexcept { return HStringView(*this) <=> str; }
	inline std::strong_ordering HString::operator<=>(HStringView str) const noexcept { return HStringView(*this) <=> str; }
	inline std::strong_ordering HString::operator<=>(const HString& str) const noexcept { return HStringView(*this) <=> HStringView(str); }
}

// iterator
namespace hana
{
	inline HString::pointer HString::begin() noexcept { return data(); }
	inline HString::const_pointer HString::begin() const noexcept { return data(); }
	inline HString::const_pointer HString::cbegin() const noexcept { return data(); }

	inline HString::pointer HString::end() noexcept { return data() + size(); }
	inline HString::const_pointer HString::end() const noexcept { return data() + size(); }
	inline HString::const_pointer HString::cend() const noexcept { return data() + size(); }

	inline std::reverse_iterator<HString::pointer> HString::rbegin() noexcept { return std::reverse_iterator(end()); }
	inline std::reverse_iterator<HString::const_pointer> HString::rbegin() const noexcept { return std::reverse_iterator(end()); }
	inline std::reverse_iterator<HString::const_pointer> HString::crbegin() const noexcept { return std::reverse_iterator(end()); }

	inline std::reverse_iterator<HString::pointer> HString::rend() noexcept { return std::reverse_iterator(begin()); }
	inline std::reverse_iterator<HString::const_pointer> HString::rend() const noexcept { return std::reverse_iterator(begin()); }
	inline std::reverse_iterator<HString::const_pointer> HString::crend() const noexcept { return std::reverse_iterator(begin()); }

	inline HString::cursor HString::cursor_begin() { return cursor::Begin(data(), size()); }
	inline HString::cursor HString::cursor_end() { return cursor::End(data(), size()); }
	inline HString::iterator HString::iter() { return cursor::Begin(data(), size()).as_iter(); }
	inline HString::reverse_iterator HString::iter_inv() { return cursor::End(data(), size()).as_iter_inv(); }
	inline HString::range_t HString::range() { return cursor::Begin(data(), size()).as_range(); }
	inline HString::reverse_range HString::range_inv() { return cursor::End(data(), size()).as_range_inv(); }

	inline HString::const_cursor HString::cursor_begin() const { return const_cursor::Begin(data(), size()); }
	inline HString::const_cursor HString::cursor_end() const { return const_cursor::End(data(), size()); }
	inline HString::const_iterator HString::iter() const { return const_cursor::Begin(data(), size()).as_iter(); }
	inline HString::const_reverse_iterator HString::iter_inv() const { return const_cursor::End(data(), size()).as_iter_inv(); }
	inline HString::const_range HString::range() const { return const_cursor::Begin(data(), size()).as_range(); }
	inline HString::const_reverse_range HString::range_inv() const { return const_cursor::End(data(), size()).as_range_inv(); }
}

// size
namespace hana
{
	inline bool HString::empty() const noexcept {
		return !size();
	}

	inline HString::size_type HString::length() const noexcept {
		return size();
	}

	inline HString::size_type HString::text_length() const noexcept {
		return HStringView(*this).text_length();
	}

	inline HString::size_type HString::capacity() const noexcept {
		return is_sso() ? SSOCapacity : capacity_;
	}

	inline HString::size_type HString::slack() const noexcept {
		return capacity() - size();
	}

	inline HString::size_type HString::size() const noexcept {
		return is_sso() ? sso_size_ : size_;
	}

	inline HString::size_type HString::max_size() const noexcept {
		return HStringView(*this).max_size();
	}

	template<is_char_v Char>
	HString::size_type HString::to_size() const noexcept {
		return HStringView(*this).to_size<Char>();
	}
}

// access
namespace hana
{
	inline HString::reference HString::at(size_type pos) {
		assert(is_valid_index(pos));
		return data()[pos];
	}

	inline HString::reference HString::operator[](size_type pos) {
		return at(pos);
	}

	inline HString::reference HString::front() {
		return at(0);
	}

	inline HString::reference HString::back() {
		return at(size() - 1);
	}

	inline HString::pointer HString::data() noexcept {
		return is_sso() ? sso_data_ : data_;
	}

	inline HString::const_reference HString::at(size_type pos) const {
		assert(is_valid_index(pos));
		return data()[pos];
	}

	inline HString::const_reference HString::operator[](size_type pos) const {
		return at(pos);
	}

	inline HString::const_reference HString::front() const {
		return at(0);
	}

	inline HString::const_reference HString::back() const {
		return at(size() - 1);
	}

	inline HString::const_pointer HString::data() const noexcept {
		return is_sso() ? sso_data_ : data_;
	}

	inline HString::raw_reference HString::raw_at(size_type pos) {
		assert(is_valid_index(pos));
		return raw_data()[pos];
	}

	inline HString::raw_reference HString::raw_front() {
		return raw_at(0);
	}

	inline HString::raw_reference HString::raw_back() {
		return raw_at(size() - 1);
	}

	inline HString::raw_pointer HString::raw_data() noexcept {
		return reinterpret_cast<raw_pointer>(data());
	}

	inline HString::raw_const_reference HString::raw_at(size_type pos) const {
		assert(is_valid_index(pos));
		return raw_data()[pos];
	}

	inline HString::raw_const_reference HString::raw_front() const {
		return raw_at(0);
	}

	inline HString::raw_const_reference HString::raw_back() const {
		return raw_at(size() - 1);
	}

	inline HString::raw_const_pointer HString::raw_data() const noexcept {
		return reinterpret_cast<raw_const_pointer>(data());
	}

	inline HString::const_pointer HString::c_str() const noexcept {
		return data();
	}

	inline unicode::UTF8Seq HString::at_text(size_type index) const {
		return HStringView(*this).at_text(index);
	}

	inline unicode::UTF8Seq HString::last_text(size_type index) const {
		return HStringView(*this).last_text(index);
	}

	inline bool HString::is_sso() const noexcept {
		return sso_flag_;
	}

	inline bool HString::is_heap() const noexcept {
		return !sso_flag_;
	}

	inline bool HString::is_valid_index(size_type index) const noexcept {
		return HStringView(*this).is_valid_index(index);
	}

	inline HString::size_type HString::buffer_index_to_text(size_type index) const noexcept {
		return HStringView(*this).buffer_index_to_text(index);
	}

	inline HString::size_type HString::text_index_to_buffer(size_type index) const noexcept {
		return HStringView(*this).text_index_to_buffer(index);
	}
}

// substr
namespace hana
{
	inline HString::operator HStringView() const noexcept {
		return {data(), size()};
	}

	inline HStringView HString::first_view(size_type count) const {
		return HStringView(*this).first_view(count);
	}

	inline HStringView HString::last_view(size_type count) const {
		return HStringView(*this).last_view(count);
	}

	inline HStringView HString::subview(size_type start, size_type count) const noexcept {
		return HStringView(*this).subview(start, count);
	}

	inline HString HString::first_str(size_type count) const {
		return HString{first_view(count)};
	}

	inline HString HString::last_str(size_type count) const {
		return HString{last_view(count)};
	}

	inline HString HString::substr(size_type pos, size_type count) const {
		return HString{subview(pos, count)};
	}
}

// add
namespace hana
{
	inline HString& HString::insert(size_type index, unicode::UTF8Seq seq) {
		assert(index <= size());
		if (seq.is_valid()) {
			return insert(index, HStringView{seq.data, seq.len});
		}
		return *this;
	}

	inline HString& HString::insert(size_type index, const wchar_t* str) {
		return insert(index, reinterpret_cast<internal::const_wchar_ptr>(str));
	}

	inline HString& HString::append(unicode::UTF8Seq seq) {
		if (seq.is_valid()) {
			return append(HStringView(seq.data, seq.len));
		}
		return *this;
	}

	inline HString& HString::append(const wchar_t* str) {
		return append(reinterpret_cast<internal::const_wchar_ptr>(str));
	}

	inline HString& HString::push_back(value_type ch) {
		return append(1, ch);
	}

	inline void HString::operator+=(value_type ch) {
		push_back(ch);
	}

	inline void HString::operator+=(unicode::UTF8Seq seq) {
		append(seq);
	}

	inline void HString::operator+=(HStringView view) {
		append(view);
	}

	inline void HString::operator+=(const char16_t* str) {
		append(str);
	}

	inline void HString::operator+=(const char32_t* str) {
		append(str);
	}

	inline void HString::operator+=(const wchar_t* str) {
		append(str);
	}
}

// replace
namespace hana
{
	inline HString& HString::replace(size_type pos, size_type count, HStringView view) {
		return replace(pos, count, view.data(), view.size());
	}

	inline HString HString::Replace(size_type pos, size_type count, const_pointer cstr, size_type count2) const {
		return HString(*this).replace(pos, count, cstr, count2);
	}

	inline HString HString::Replace(size_type pos, size_type count, HStringView view) const {
		return HString(*this).replace(pos, count, view);
	}
}

// starts with
namespace hana
{
	inline bool HString::starts_with(HStringView sv) const noexcept {
		return HStringView(*this).starts_with(sv);
	}

	inline bool HString::starts_with(value_type ch) const noexcept {
		return HStringView(*this).starts_with(ch);
	}

	inline bool HString::starts_with(unicode::UTF8Seq seq) const {
		return HStringView(*this).starts_with(seq);
	}

	inline bool HString::ends_with(HStringView sv) const noexcept {
		return HStringView(*this).ends_with(sv);
	}

	inline bool HString::ends_with(value_type ch) const noexcept {
		return HStringView(*this).ends_with(ch);
	}

	inline bool HString::ends_with(unicode::UTF8Seq seq) const {
		return HStringView(*this).ends_with(seq);
	}
}

// contains & count
namespace hana
{
	inline bool HString::contains(HStringView sv) const noexcept {
		return HStringView(*this).contains(sv);
	}

	inline bool HString::contains(value_type ch) const noexcept {
		return HStringView(*this).contains(ch);
	}

	inline bool HString::contains(unicode::UTF8Seq seq) const {
		return HStringView(*this).contains(seq);
	}

	inline HString::size_type HString::count(HStringView pattern) const {
		return HStringView(*this).count(pattern);
	}

	inline HString::size_type HString::count(unicode::UTF8Seq seq) const {
		return HStringView(*this).count(seq);
	}
}

// find
namespace hana
{
#define HANA_STRING_FIND(name)																																\
	inline HString::data_reference HString::name(HStringView v, size_type pos) noexcept { return HStringView(*this).name(v, pos); }							\
	inline HString::data_reference HString::name(value_type ch, size_type pos) noexcept { return HStringView(*this).name(ch, pos); }						\
	inline HString::data_reference HString::name(unicode::UTF8Seq seq, size_type pos) { return HStringView(*this).name(seq, pos); }							\
	inline HString::data_reference HString::name(const_pointer s, size_type pos, size_type count) { return HStringView(*this).name(s, pos, count); }		\
	inline HString::const_data_reference HString::name(HStringView v, size_type pos) const noexcept { return HStringView(*this).name(v, pos); }				\
	inline HString::const_data_reference HString::name(value_type ch, size_type pos) const noexcept { return HStringView(*this).name(ch, pos); }			\
	inline HString::const_data_reference HString::name(unicode::UTF8Seq seq, size_type pos) const { return HStringView(*this).name(seq, pos); }				\
	inline HString::const_data_reference HString::name(const_pointer s, size_type pos, size_type count) const { return HStringView(*this).name(s, pos, count); }

	HANA_STRING_FIND(find)
	HANA_STRING_FIND(find_first_of)
	HANA_STRING_FIND(find_first_not_of)
	HANA_STRING_FIND(rfind)
	HANA_STRING_FIND(find_last_of)
	HANA_STRING_FIND(find_last_not_of)

#undef HANA_STRING_FIND
}

// remove prefix & prefix
namespace hana
{
	inline HString& HString::remove_prefix(size_type n) {
		return erase(0, n);
	}

	inline HString& HString::remove_prefix(HStringView prefix) {
		if (starts_with(prefix)) {
			return erase(0, prefix.size());
		}
		return *this;
	}

	inline HString& HString::remove_prefix(unicode::UTF8Seq prefix) {
		if (prefix.is_valid()) {
			return remove_prefix(HStringView(prefix.data, prefix.len));
		}
		return *this;
	}

	inline HString& HString::remove_suffix(size_type n) {
		return erase(size() - n);
	}

	inline HString& HString::remove_suffix(HStringView suffix) {
		if (ends_with(suffix)) {
			return erase(size() - suffix.size());
		}
		return *this;
	}

	inline HString& HString::remove_suffix(unicode::UTF8Seq suffix) {
		if (suffix.is_valid()) {
			return remove_suffix(HStringView(suffix.data, suffix.len));
		}
		return *this;
	}

	inline HString HString::RemovePrefix(size_type n) const {
		return HString{HStringView(*this).RemovePrefix(n)};
	}

	inline HString HString::RemovePrefix(HStringView prefix) const {
		return HString{HStringView(*this).RemovePrefix(prefix)};
	}

	inline HString HString::RemovePrefix(unicode::UTF8Seq prefix) const {
		return HString{HStringView(*this).RemovePrefix(prefix)};
	}

	inline HString HString::RemoveSuffix(size_type n) const {
		return HString{HStringView(*this).RemoveSuffix(n)};
	}

	inline HString HString::RemoveSuffix(HStringView suffix) const {
		return HString{HStringView(*this).RemoveSuffix(suffix)};
	}

	inline HString HString::RemoveSuffix(unicode::UTF8Seq suffix) const {
		return HString{HStringView(*this).RemoveSuffix(suffix)};
	}
}

// partition
namespace hana
{
	inline std::array<HStringView, 3> HString::partition(HStringView delimiter) const {
		return HStringView(*this).partition(delimiter);
	}

	inline std::array<HStringView, 3> HString::partition(unicode::UTF8Seq delimiter) const {
		return HStringView(*this).partition(delimiter);
	}
}

// trim
namespace hana
{
	inline HString& HString::trim(HStringView characters) {
		return trim_start(characters).trim_end(characters);
	}

	inline HString& HString::trim_start(HStringView characters) {
		auto target = HStringView(*this).trim_start(characters);
		auto count = size() - target.size();
		return count ? erase(0, count) : *this;
	}

	inline HString& HString::trim_end(HStringView characters) {
		auto target = HStringView(*this).trim_end(characters);
		return target.size() == size() ? *this : erase(target.size());
	}

	inline HString& HString::trim(unicode::UTF8Seq seq) {
		if (seq.is_valid()) {
			return trim(HStringView(seq.data, seq.len));
		}
		return *this;
	}

	inline HString& HString::trim_start(unicode::UTF8Seq seq) {
		if (seq.is_valid()) {
			return trim_start(HStringView(seq.data, seq.len));
		}
		return *this;
	}

	inline HString& HString::trim_end(unicode::UTF8Seq seq) {
		if (seq.is_valid()) {
			return trim_end(HStringView(seq.data, seq.len));
		}
		return *this;
	}

	inline HString& HString::trim_invalid() {
		return trim_invalid_start().trim_invalid_end();
	}

	inline HString& HString::trim_invalid_start() {
		auto target = HStringView(*this).trim_invalid_start();
		auto count = size() - target.size();
		return count ? erase(0, count) : *this;
	}

	inline HString& HString::trim_invalid_end() {
		auto target = HStringView(*this).trim_invalid_end();
		return target.size() == size() ? *this : erase(target.size());
	}

	inline HString HString::Trim(HStringView characters) const {
		return HString{HStringView(*this).trim(characters)};
	}

	inline HString HString::TrimStart(HStringView characters) const {
		return HString{HStringView(*this).trim_start(characters)};
	}

	inline HString HString::TrimEnd(HStringView characters) const {
		return HString{HStringView(*this).trim_end(characters)};
	}

	inline HString HString::Trim(unicode::UTF8Seq seq) const {
		return HString{HStringView(*this).trim(seq)};
	}

	inline HString HString::TrimStart(unicode::UTF8Seq seq) const {
		return HString{HStringView(*this).trim_start(seq)};
	}

	inline HString HString::TrimEnd(unicode::UTF8Seq seq) const {
		return HString{HStringView(*this).trim_end(seq)};
	}

	inline HString HString::TrimInvalid() const {
		return HString{HStringView(*this).trim_invalid()};
	}

	inline HString HString::TrimInvalidStart() const {
		return HString{HStringView(*this).trim_invalid_start()};
	}

	inline HString HString::TrimInvalidEnd() const {
		return HString{HStringView(*this).trim_invalid_end()};
	}
}

// split
namespace hana
{
	template<internal::CanAdd<HStringView> Buffer>
	HString::size_type HString::split(Buffer& out, HStringView delimiter, bool cull_empty, size_type limit) const {
		return HStringView(*this).split(out, delimiter, cull_empty, limit);
	}

	template<std::invocable<HStringView> F>
	HString::size_type HString::split_each(F&& func, HStringView delimiter, bool cull_empty, size_type limit) const {
		return HStringView(*this).split(std::forward<F>(func), delimiter, cull_empty, limit);
	}

	template<internal::CanAdd<HStringView> Buffer>
	HString::size_type HString::split(Buffer& out, unicode::UTF8Seq delimiter, bool cull_empty, size_type limit) const {
		return HStringView(*this).split(out, delimiter, cull_empty, limit);
	}

	template<std::invocable<HStringView> F>
	HString::size_type HString::split_each(F&& func, unicode::UTF8Seq delimiter, bool cull_empty, size_type limit) const {
		return HStringView(*this).split(std::forward<F>(func), delimiter, cull_empty, limit);
	}
}

// misc
namespace hana
{
	inline void HString::release(size_type reserve_capacity) {
		reserve(reserve_capacity);
		clear();
	}

	inline void HString::swap(HString& other) noexcept {
		std::swap(buffer_, other.buffer_);
	}

	inline HString& HString::reverse(size_type start, size_type count) {
		assert(is_valid_index(start) && "undefined behaviour accessing out of bounds");
		assert(count == npos || count <= size() - start && "undefined behaviour exceeding size of string view");
		count = count == npos ? size() - start : count;

		std::reverse(data() + start, data() + start + count);
		return *this;
	}

	inline HString::size_type HString::copy(pointer dest, size_type count, size_type pos) const {
		return HStringView(*this).copy(dest, count, pos);
	}
}
