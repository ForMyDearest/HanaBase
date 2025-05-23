#pragma once

#include "hana/unicode/iterator.hpp"

namespace hana
{
	/*!
	 * @note
	 *		1.Not support constexpr char operation \n
	 *		2.It's not a good idea to support implicit type conversion with std::basic_string_view
	 *		as std::basic_string_view allows for implicit type conversion with cstring,
	 *		which can lead to \b ambiguity in the final construction
	 */
	class HStringView {
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

		using const_data_reference = container::VectorDataRef<value_type, true>;
		using const_cursor = unicode::UTF8Cursor<true>;
		using const_iterator = unicode::UTF8Iter<true>;
		using const_reverse_iterator = unicode::UTF8IterInv<true>;
		using const_range = unicode::UTF8Range<true>;
		using const_reverse_range = unicode::UTF8RangeInv<true>;

		using traits_type = std::char_traits<value_type>;

		static constexpr size_type npos = const_data_reference::npos;

		//==================> ctor & dtor <==================

		constexpr HStringView() noexcept = default;
		constexpr HStringView(HStringView&& other) noexcept = default;
		constexpr HStringView(const HStringView& other) noexcept = default;
		constexpr HStringView(const_pointer str);
		constexpr HStringView(const_pointer str, size_type count);
		HStringView(raw_const_pointer str);
		HStringView(raw_const_pointer str, size_type count);
		constexpr explicit HStringView(std::nullptr_t) = delete;
		constexpr ~HStringView() noexcept = default;

		//==================> assign <==================

		constexpr HStringView& operator=(HStringView&& rhs) noexcept = default;
		constexpr HStringView& operator=(const HStringView& rhs) noexcept = default;

		//==================> compare <==================

		constexpr bool operator==(const HStringView& rhs) const noexcept;
		constexpr std::strong_ordering operator<=>(const HStringView& rhs) const noexcept;

		constexpr int compare(HStringView v) const noexcept;
		constexpr int compare(size_type pos1, size_type count1, HStringView v) const;
		constexpr int compare(size_type pos1, size_type count1, HStringView v, size_type pos2, size_type count2) const;
		constexpr int compare(size_type pos1, size_type count1, const_pointer s, size_type count2) const;

		//==================> iterator <==================

		constexpr const_pointer begin() const noexcept;
		constexpr const_pointer cbegin() const noexcept;
		constexpr const_pointer end() const noexcept;
		constexpr const_pointer cend() const noexcept;
		constexpr std::reverse_iterator<const_pointer> rbegin() const noexcept;
		constexpr std::reverse_iterator<const_pointer> crbegin() const noexcept;
		constexpr std::reverse_iterator<const_pointer> rend() const noexcept;
		constexpr std::reverse_iterator<const_pointer> crend() const noexcept;

		raw_const_pointer raw_begin() const noexcept;
		raw_const_pointer raw_cbegin() const noexcept;
		raw_const_pointer raw_end() const noexcept;
		raw_const_pointer raw_cend() const noexcept;
		std::reverse_iterator<raw_const_pointer> raw_rbegin() const noexcept;
		std::reverse_iterator<raw_const_pointer> raw_crbegin() const noexcept;
		std::reverse_iterator<raw_const_pointer> raw_rend() const noexcept;
		std::reverse_iterator<raw_const_pointer> raw_crend() const noexcept;

		constexpr const_cursor cursor_begin() const;
		constexpr const_cursor cursor_end() const;
		constexpr const_iterator iter() const;
		constexpr const_reverse_iterator iter_inv() const;
		constexpr const_range range() const;
		constexpr const_reverse_range range_inv() const;

		//==================> size <==================

		constexpr bool empty() const noexcept;
		constexpr size_type length() const noexcept;
		constexpr size_type text_length() const noexcept;
		constexpr size_type size() const noexcept;
		constexpr size_type max_size() const noexcept;
		template<is_char_v Char> constexpr size_type to_size() const noexcept;

		//==================> data access <==================

		constexpr const_reference at(size_type pos) const;
		constexpr const_reference front() const;
		constexpr const_reference back() const;
		constexpr const_pointer data() const noexcept;
		constexpr std::u8string_view view() const noexcept;

		raw_const_reference raw_at(size_type pos) const;
		raw_const_reference raw_front() const;
		raw_const_reference raw_back() const;
		raw_const_pointer raw_data() const noexcept;
		std::string_view raw_view() const noexcept;

		constexpr const_reference operator[](size_type pos) const;
		constexpr unicode::UTF8Seq at_text(size_type pos) const;
		constexpr unicode::UTF8Seq last_text(size_type index) const;
		constexpr bool is_valid_index(size_type index) const noexcept;
		constexpr size_type buffer_index_to_text(size_type index) const noexcept;
		constexpr size_type text_index_to_buffer(size_type index) const noexcept;

		//==================> remove prefix & prefix <==================

		constexpr void remove_prefix(size_type n);
		constexpr void remove_prefix(HStringView prefix);
		constexpr void remove_prefix(unicode::UTF8Seq prefix);

		constexpr HStringView RemovePrefix(size_type n) const;
		constexpr HStringView RemovePrefix(HStringView prefix) const;
		constexpr HStringView RemovePrefix(unicode::UTF8Seq prefix) const;

		constexpr void remove_suffix(size_type n);
		constexpr void remove_suffix(HStringView suffix);
		constexpr void remove_suffix(unicode::UTF8Seq suffix);

		constexpr HStringView RemoveSuffix(size_type n) const;
		constexpr HStringView RemoveSuffix(HStringView suffix) const;
		constexpr HStringView RemoveSuffix(unicode::UTF8Seq suffix) const;

		//==================> subview <==================

		constexpr HStringView first_view(size_type count) const;
		constexpr HStringView last_view(size_type count) const;
		constexpr HStringView subview(size_type pos = 0, size_type count = npos) const;

		//==================> starts & ends with <==================

		constexpr bool starts_with(HStringView sv) const noexcept;
		constexpr bool starts_with(value_type ch) const noexcept;
		constexpr bool starts_with(unicode::UTF8Seq prefix) const;

		constexpr bool ends_with(HStringView sv) const noexcept;
		constexpr bool ends_with(value_type ch) const noexcept;
		constexpr bool ends_with(unicode::UTF8Seq suffix) const;

		//==================> contains & count <==================

		constexpr bool contains(HStringView sv) const noexcept;
		constexpr bool contains(value_type ch) const noexcept;
		constexpr bool contains(unicode::UTF8Seq seq) const;

		constexpr size_type count(HStringView pattern) const;
		constexpr size_type count(unicode::UTF8Seq seq) const;

		//==================> find <==================

		constexpr const_data_reference find(HStringView v, size_type pos = 0) const noexcept;
		constexpr const_data_reference find(value_type ch, size_type pos = 0) const noexcept;
		constexpr const_data_reference find(unicode::UTF8Seq pattern, size_type pos = 0) const;
		constexpr const_data_reference find(const_pointer s, size_type pos, size_type count) const;

		constexpr const_data_reference find_first_of(HStringView v, size_type pos = 0) const noexcept;
		constexpr const_data_reference find_first_of(value_type ch, size_type pos = 0) const noexcept;
		constexpr const_data_reference find_first_of(unicode::UTF8Seq pattern, size_type pos = 0) const;
		constexpr const_data_reference find_first_of(const_pointer s, size_type pos, size_type count) const;

		constexpr const_data_reference find_first_not_of(HStringView v, size_type pos = 0) const noexcept;
		constexpr const_data_reference find_first_not_of(value_type ch, size_type pos = 0) const noexcept;
		constexpr const_data_reference find_first_not_of(unicode::UTF8Seq pattern, size_type pos = 0) const;
		constexpr const_data_reference find_first_not_of(const_pointer s, size_type pos, size_type count) const;

		constexpr const_data_reference rfind(HStringView v, size_type pos = npos) const noexcept;
		constexpr const_data_reference rfind(value_type ch, size_type pos = npos) const noexcept;
		constexpr const_data_reference rfind(unicode::UTF8Seq pattern, size_type pos = npos) const;
		constexpr const_data_reference rfind(const_pointer s, size_type pos, size_type count) const;

		constexpr const_data_reference find_last_of(HStringView v, size_type pos = npos) const noexcept;
		constexpr const_data_reference find_last_of(value_type ch, size_type pos = npos) const noexcept;
		constexpr const_data_reference find_last_of(unicode::UTF8Seq pattern, size_type pos = npos) const;
		constexpr const_data_reference find_last_of(const_pointer s, size_type pos, size_type count) const;

		constexpr const_data_reference find_last_not_of(HStringView v, size_type pos = npos) const noexcept;
		constexpr const_data_reference find_last_not_of(value_type ch, size_type pos = npos) const noexcept;
		constexpr const_data_reference find_last_not_of(unicode::UTF8Seq pattern, size_type pos = npos) const;
		constexpr const_data_reference find_last_not_of(const_pointer s, size_type pos, size_type count) const;

		//==================> partition <==================

		constexpr std::array<HStringView, 3> partition(HStringView delimiter) const;
		constexpr std::array<HStringView, 3> partition(unicode::UTF8Seq delimiter) const;

		//==================> trim <==================

		constexpr HStringView trim(HStringView characters = u8" \t") const;
		constexpr HStringView trim_start(HStringView characters = u8" \t") const;
		constexpr HStringView trim_end(HStringView characters = u8" \t") const;
		constexpr HStringView trim(unicode::UTF8Seq seq) const;
		constexpr HStringView trim_start(unicode::UTF8Seq seq) const;
		constexpr HStringView trim_end(unicode::UTF8Seq seq) const;
		constexpr HStringView trim_invalid() const;
		constexpr HStringView trim_invalid_start() const;
		constexpr HStringView trim_invalid_end() const;

		//==================> split <==================

		template<internal::CanAdd<HStringView> Buffer>
		constexpr size_type split(Buffer& out, HStringView delimiter, bool cull_empty = false, size_type limit = npos) const;

		template<internal::CanAdd<HStringView> Buffer>
		constexpr size_type split(Buffer& out, unicode::UTF8Seq delimiter, bool cull_empty = false, size_type limit = npos) const;

		template<std::invocable<HStringView> F>
		constexpr size_type split_each(F&& func, HStringView delimiter, bool cull_empty = false, size_type limit = npos) const;

		template<std::invocable<HStringView> F>
		constexpr size_type split_each(F&& func, unicode::UTF8Seq delimiter, bool cull_empty = false, size_type limit = npos) const;

		//==================> misc <==================

		constexpr void swap(HStringView& v) noexcept;
		constexpr size_type copy(pointer dest, size_type count, size_type pos = 0) const;

	private:
		std::u8string_view data_;
	};
}

// ctor & dtor
namespace hana
{
	constexpr HStringView::HStringView(const_pointer str) : data_(str) {}
	constexpr HStringView::HStringView(const_pointer str, size_type count): data_(str, count) {}
	inline HStringView::HStringView(raw_const_pointer str) : data_(reinterpret_cast<const_pointer>(str)) {}
	inline HStringView::HStringView(raw_const_pointer str, size_type count) : data_(reinterpret_cast<const_pointer>(str), count) {}
}

// compare
namespace hana
{
	constexpr bool HStringView::operator==(const HStringView& rhs) const noexcept {
		return data_ == rhs.data_;
	}

	constexpr std::strong_ordering HStringView::operator<=>(const HStringView& rhs) const noexcept {
		return data_ <=> rhs.data_;
	}

	constexpr int HStringView::compare(HStringView v) const noexcept {
		return data_.compare(v.data_);
	}

	constexpr int HStringView::compare(size_type pos1, size_type count1, HStringView v) const {
		return data_.compare(pos1, count1, v.data_);
	}

	constexpr int HStringView::compare(size_type pos1, size_type count1, HStringView v, size_type pos2, size_type count2) const {
		return data_.compare(pos1, count1, v.data_, pos2, count2);
	}

	constexpr int HStringView::compare(size_type pos1, size_type count1, const_pointer s, size_type count2) const {
		return data_.compare(pos1, count1, s, count2);
	}
}

// iterator
namespace hana
{
	constexpr HStringView::const_pointer HStringView::begin() const noexcept { return data(); }
	constexpr HStringView::const_pointer HStringView::cbegin() const noexcept { return data(); }
	constexpr HStringView::const_pointer HStringView::end() const noexcept { return data() + size(); }
	constexpr HStringView::const_pointer HStringView::cend() const noexcept { return data() + size(); }
	constexpr std::reverse_iterator<HStringView::const_pointer> HStringView::rbegin() const noexcept { return std::reverse_iterator{end()}; }
	constexpr std::reverse_iterator<HStringView::const_pointer> HStringView::crbegin() const noexcept { return std::reverse_iterator{cend()}; }
	constexpr std::reverse_iterator<HStringView::const_pointer> HStringView::rend() const noexcept { return std::reverse_iterator{begin()}; }
	constexpr std::reverse_iterator<HStringView::const_pointer> HStringView::crend() const noexcept { return std::reverse_iterator{cbegin()}; }

	inline HStringView::raw_const_pointer HStringView::raw_begin() const noexcept { return raw_data(); }
	inline HStringView::raw_const_pointer HStringView::raw_cbegin() const noexcept { return raw_data(); }
	inline HStringView::raw_const_pointer HStringView::raw_end() const noexcept { return raw_data() + size(); }
	inline HStringView::raw_const_pointer HStringView::raw_cend() const noexcept { return raw_data() + size(); }
	inline std::reverse_iterator<HStringView::raw_const_pointer> HStringView::raw_rbegin() const noexcept { return std::reverse_iterator{raw_end()}; }
	inline std::reverse_iterator<HStringView::raw_const_pointer> HStringView::raw_crbegin() const noexcept { return std::reverse_iterator{raw_cend()}; }
	inline std::reverse_iterator<HStringView::raw_const_pointer> HStringView::raw_rend() const noexcept { return std::reverse_iterator{raw_begin()}; }
	inline std::reverse_iterator<HStringView::raw_const_pointer> HStringView::raw_crend() const noexcept { return std::reverse_iterator{raw_cbegin()}; }

	constexpr HStringView::const_cursor HStringView::cursor_begin() const { return const_cursor::Begin(data(), size()); }
	constexpr HStringView::const_cursor HStringView::cursor_end() const { return const_cursor::End(data(), size()); }
	constexpr HStringView::const_iterator HStringView::iter() const { return const_cursor::Begin(data(), size()).as_iter(); }
	constexpr HStringView::const_reverse_iterator HStringView::iter_inv() const { return const_cursor::End(data(), size()).as_iter_inv(); }
	constexpr HStringView::const_range HStringView::range() const { return const_cursor::Begin(data(), size()).as_range(); }
	constexpr HStringView::const_reverse_range HStringView::range_inv() const { return const_cursor::End(data(), size()).as_range_inv(); }
}

// size
namespace hana
{
	constexpr bool HStringView::empty() const noexcept { return data_.empty(); }
	constexpr HStringView::size_type HStringView::length() const noexcept { return data_.length(); }
	constexpr HStringView::size_type HStringView::text_length() const noexcept { return empty() ? 0 : unicode::utf8_code_point_index(data(), size(), size() - 1) + 1; }
	constexpr HStringView::size_type HStringView::size() const noexcept { return data_.size(); }
	constexpr HStringView::size_type HStringView::max_size() const noexcept { return data_.max_size(); }

	template<is_char_v Char>
	constexpr HStringView::size_type HStringView::to_size() const noexcept {
		if constexpr (sizeof(Char) == sizeof(char8_t)) {
			return size();
		} else if constexpr (sizeof(Char) == sizeof(char16_t)) {
			if (empty()) {
				return 0;
			}

			using Cursor = unicode::UTF8Cursor<true>;

			size_type utf16_len = 0;
			for (unicode::UTF8Seq utf8_seq: Cursor{data(), size(), 0}.as_range()) {
				if (utf8_seq.is_valid()) {
					utf16_len += utf8_seq.to_utf16_len();
				} else {
					utf16_len += 1;
				}
			}

			return utf16_len;
		} else {
			if (empty()) {
				return 0;
			}

			using Cursor = unicode::UTF8Cursor<true>;
			size_type utf32_len = 0;
			for ([[maybe_unused]] unicode::UTF8Seq utf8_seq: Cursor{data(), size(), 0}.as_range()) {
				utf32_len += 1;
			}
			return utf32_len;
		}
	}
}

// access
namespace hana
{
	constexpr HStringView::const_reference HStringView::at(size_type pos) const { return data_.at(pos); }
	constexpr HStringView::const_reference HStringView::front() const { return data_.front(); }
	constexpr HStringView::const_reference HStringView::back() const { return data_.back(); }
	constexpr HStringView::const_pointer HStringView::data() const noexcept { return data_.data(); }
	constexpr std::u8string_view HStringView::view() const noexcept { return data_; }

	inline HStringView::raw_const_reference HStringView::raw_at(size_type pos) const { return *reinterpret_cast<raw_const_pointer>(&at(pos)); }
	inline HStringView::raw_const_reference HStringView::raw_front() const { return *reinterpret_cast<raw_const_pointer>(&front()); }
	inline HStringView::raw_const_reference HStringView::raw_back() const { return *reinterpret_cast<raw_const_pointer>(&back()); }
	inline HStringView::raw_const_pointer HStringView::raw_data() const noexcept { return reinterpret_cast<raw_const_pointer>(data()); }
	inline std::string_view HStringView::raw_view() const noexcept { return {reinterpret_cast<raw_const_pointer>(data()), size()}; }

	constexpr HStringView::const_reference HStringView::operator[](size_type pos) const { return data_[pos]; }

	constexpr unicode::UTF8Seq HStringView::at_text(size_type pos) const {
		assert(this->is_valid_index(pos) && "undefined behavior accessing an empty string view");
		uint64_t seq_index;
		return unicode::UTF8Seq::ParseUTF8(data(), size(), pos, seq_index);
	}

	constexpr unicode::UTF8Seq HStringView::last_text(size_type index) const {
		return at_text(size() - index - 1);
	}

	constexpr bool HStringView::is_valid_index(size_type index) const noexcept {
		return index < size();
	}

	constexpr HStringView::size_type HStringView::buffer_index_to_text(size_type index) const noexcept {
		return unicode::utf8_code_point_index(data(), size(), index);
	}

	constexpr HStringView::size_type HStringView::text_index_to_buffer(size_type index) const noexcept {
		return unicode::utf8_code_unit_index(data(), size(), index);
	}
}

// remove prefix & prefix
namespace hana
{
	constexpr void HStringView::remove_prefix(size_type n) {
		data_.remove_prefix(n);
	}

	constexpr void HStringView::remove_prefix(HStringView prefix) {
		if (starts_with(prefix)) remove_prefix(prefix.size());
	}

	constexpr void HStringView::remove_prefix(unicode::UTF8Seq prefix) {
		if (prefix.is_valid()) {
			remove_prefix(HStringView{prefix.data, prefix.len});
		}
	}

	constexpr HStringView HStringView::RemovePrefix(size_type n) const {
		return subview(n);
	}

	constexpr HStringView HStringView::RemovePrefix(HStringView prefix) const {
		return starts_with(prefix) ? RemovePrefix(prefix.size()) : *this;
	}

	constexpr HStringView HStringView::RemovePrefix(unicode::UTF8Seq prefix) const {
		return prefix.is_valid() ? RemovePrefix(HStringView{prefix.data, prefix.len}) : *this;
	}

	constexpr void HStringView::remove_suffix(size_type n) {
		data_.remove_suffix(n);
	}

	constexpr void HStringView::remove_suffix(HStringView suffix) {
		if (ends_with(suffix)) remove_suffix(suffix.size());
	}

	constexpr void HStringView::remove_suffix(unicode::UTF8Seq suffix) {
		if (suffix.is_valid()) {
			remove_suffix(HStringView{suffix.data, suffix.len});
		}
	}

	constexpr HStringView HStringView::RemoveSuffix(size_type n) const {
		return subview(0, size() - n);
	}

	constexpr HStringView HStringView::RemoveSuffix(HStringView suffix) const {
		return ends_with(suffix) ? RemoveSuffix(suffix.size()) : *this;
	}

	constexpr HStringView HStringView::RemoveSuffix(unicode::UTF8Seq suffix) const {
		return suffix.is_valid() ? RemoveSuffix(HStringView{suffix.data, suffix.len}) : *this;
	}
}

// subview
namespace hana
{
	constexpr HStringView HStringView::first_view(size_type count) const {
		assert(count <= size() && "undefined behavior accessing out of bounds");
		return {data(), count};
	}

	constexpr HStringView HStringView::last_view(size_type count) const {
		assert(count <= size() && "undefined behavior accessing out of bounds");
		return {data() + size() - count, count};
	}

	constexpr HStringView HStringView::subview(size_type pos, size_type count) const {
		auto sv = data_.substr(pos, count);
		return {sv.data(), sv.size()};
	}
}

// starts & ends with
namespace hana
{
	constexpr bool HStringView::starts_with(HStringView sv) const noexcept {
		return data_.starts_with(sv.data_);
	}

	constexpr bool HStringView::starts_with(value_type ch) const noexcept {
		return data_.starts_with(ch);
	}

	constexpr bool HStringView::starts_with(unicode::UTF8Seq prefix) const {
		if (prefix.is_valid()) {
			return starts_with(HStringView{prefix.data, prefix.len});
		}
		return false;
	}

	constexpr bool HStringView::ends_with(HStringView sv) const noexcept {
		return data_.ends_with(sv.data_);
	}

	constexpr bool HStringView::ends_with(value_type ch) const noexcept {
		return data_.ends_with(ch);
	}

	constexpr bool HStringView::ends_with(unicode::UTF8Seq suffix) const {
		if (suffix.is_valid()) {
			return ends_with(HStringView{suffix.data, suffix.len});
		}
		return false;
	}
}

// contains & count
namespace hana
{
	constexpr bool HStringView::contains(HStringView sv) const noexcept {
		return find(sv).is_valid();
	}

	constexpr bool HStringView::contains(value_type ch) const noexcept {
		return find(ch).is_valid();
	}

	constexpr bool HStringView::contains(unicode::UTF8Seq seq) const {
		if (seq.is_valid()) {
			return contains(HStringView{seq.data, seq.len});
		}
		return false;
	}

	constexpr HStringView::size_type HStringView::count(HStringView pattern) const {
		auto find_view{*this};
		size_type result = 0;
		while (true) {
			if (const auto find_result = find_view.find(pattern)) {
				++result;
				find_view = find_view.subview(find_result + pattern.size());
			} else {
				break;
			}
		}
		return result;
	}

	constexpr HStringView::size_type HStringView::count(unicode::UTF8Seq seq) const {
		if (seq.is_valid()) {
			return count(HStringView{seq.data, seq.len});
		}
		return 0;
	}
}

// find
namespace hana
{
#define HANA_STRING_VIEW_FIND(name)																							\
	constexpr HStringView::const_data_reference HStringView::name(HStringView v, size_type pos) const noexcept {			\
		if (const auto index = data_.name(v.data_, pos); index != npos) {													\
			return {data(), index};																							\
		}																													\
		return {};																											\
	}																														\
	constexpr HStringView::const_data_reference HStringView::name(value_type ch, size_type pos) const noexcept {			\
		if (const auto index = data_.name(ch, pos); index != npos) {														\
			return {data(), index};																							\
		}																													\
		return {};																											\
	}																														\
	constexpr HStringView::const_data_reference HStringView::name(const_pointer s, size_type pos, size_type count) const {	\
		if (const auto index = data_.name(s, pos, count); index != npos) {													\
			return {data(), index};																							\
		}																													\
		return {};																											\
	}																														\
	constexpr HStringView::const_data_reference HStringView::name(unicode::UTF8Seq pattern, size_type pos) const {			\
		if (pattern.is_valid()) {																							\
			return name(HStringView{pattern.data, pattern.len}, pos);														\
		}																													\
		return {};																											\
	}

	HANA_STRING_VIEW_FIND(find)
	HANA_STRING_VIEW_FIND(find_first_of)
	HANA_STRING_VIEW_FIND(find_first_not_of)
	HANA_STRING_VIEW_FIND(rfind)
	HANA_STRING_VIEW_FIND(find_last_of)
	HANA_STRING_VIEW_FIND(find_last_not_of)

#undef HANA_STRING_VIEW_FIND
}

// partition
namespace hana
{
	constexpr std::array<HStringView, 3> HStringView::partition(HStringView delimiter) const {
		if (auto found = find(delimiter)) {
			return {
				subview(0, found),
				subview(found, delimiter.size()),
				subview(found + delimiter.size())
			};
		}
		return {*this, {}, {}};
	}

	constexpr std::array<HStringView, 3> HStringView::partition(unicode::UTF8Seq delimiter) const {
		if (delimiter.is_valid()) {
			return partition(HStringView{delimiter.data, delimiter.len});
		}
		return {*this, {}, {}};
	}
}

// trim
namespace hana
{
	constexpr HStringView HStringView::trim(HStringView characters) const {
		return trim_start(characters).trim_end(characters);
	}

	constexpr HStringView HStringView::trim_start(HStringView characters) const {
		if (empty()) {
			return {};
		}

		if (characters.empty()) {
			return {*this};
		}

		for (auto cursor = cursor_begin(); !cursor.reach_end(); cursor.move_next()) {
			if (!characters.contains(cursor.ref())) {
				return subview(cursor.index());
			}
		}
		return {};
	}

	constexpr HStringView HStringView::trim_end(HStringView characters) const {
		if (empty()) {
			return {};
		}
		if (characters.empty()) {
			return {*this};
		}

		for (auto cursor = cursor_end(); !cursor.reach_begin(); cursor.move_prev()) {
			if (!characters.contains(cursor.ref())) {
				return subview(0, cursor.index() + cursor.seq_len());
			}
		}
		return {};
	}

	constexpr HStringView HStringView::trim(unicode::UTF8Seq seq) const {
		if (seq.is_valid()) {
			return trim(HStringView{seq.data, seq.len});
		}
		return *this;
	}

	constexpr HStringView HStringView::trim_start(unicode::UTF8Seq seq) const {
		if (seq.is_valid()) {
			return trim_start(HStringView{seq.data, seq.len});
		}
		return *this;
	}

	constexpr HStringView HStringView::trim_end(unicode::UTF8Seq seq) const {
		if (seq.is_valid()) {
			return trim_end(HStringView{seq.data, seq.len});
		}
		return *this;
	}

	constexpr HStringView HStringView::trim_invalid() const {
		return trim_invalid_start().trim_invalid_end();
	}

	constexpr HStringView HStringView::trim_invalid_start() const {
		if (empty()) {
			return {};
		}

		for (auto cursor = cursor_begin(); !cursor.reach_end(); cursor.move_next()) {
			if (cursor.ref().is_valid()) {
				return subview(cursor.index());
			}
		}
		return {};
	}

	constexpr HStringView HStringView::trim_invalid_end() const {
		if (empty()) {
			return {};
		}

		for (auto cursor = cursor_end(); !cursor.reach_begin(); cursor.move_prev()) {
			if (cursor.ref().is_valid()) {
				return subview(0, cursor.index() + cursor.seq_len());
			}
		}
		return {};
	}
}

// split
namespace hana
{
	template<internal::CanAdd<HStringView> Buffer>
	constexpr HStringView::size_type HStringView::split(Buffer& out, HStringView delimiter, bool cull_empty, size_type limit) const {
		HStringView each_view{*this};
		size_type count = 0;
		while (true) {
			const auto [left, mid, right] = each_view.partition(delimiter);

			// append
			if (!cull_empty || !left.empty()) {
				++count;
				if constexpr (internal::HasAdd<Buffer, HStringView>) {
					out.add(left);
				} else if constexpr (internal::HasAppend<Buffer, HStringView>) {
					out.append(left);
				} else if constexpr (internal::HasEmplaceBack<Buffer, HStringView>) {
					out.emplace_back(left);
				} else if constexpr (internal::HasPushBack<Buffer, HStringView>) {
					out.push_back(left);
				} else {
					assert(false);
				}
			}

			// trigger end
			if (right.empty()) {
				break;
			}

			// limit break
			if (limit != npos && count >= limit) {
				break;
			}

			// update
			each_view = right;
		}
		return count;
	}

	template<internal::CanAdd<HStringView> Buffer>
	constexpr HStringView::size_type HStringView::split(Buffer& out, unicode::UTF8Seq delimiter, bool cull_empty, size_type limit) const {
		return this->split(
			out,
			delimiter.is_valid() ? HStringView{delimiter.data, delimiter.len} : HStringView{},
			cull_empty,
			limit
		);
	}

	template<std::invocable<HStringView> F>
	constexpr HStringView::size_type HStringView::split_each(F&& func, HStringView delimiter, bool cull_empty, size_type limit) const {
		using EachFuncResultType = std::invoke_result_t<F, const HStringView&>;

		HStringView each_view{*this};
		size_type count = 0;
		while (true) {
			const auto [left, mid, right] = each_view.partition(delimiter);

			// append
			if (!cull_empty || !left.empty()) {
				++count;
				if constexpr (std::is_same_v<EachFuncResultType, bool>) {
					if (!func(left)) {
						break;
					}
				} else {
					func(left);
				}
			}

			// trigger end
			if (right.empty()) {
				break;
			}

			// limit break
			if (limit != npos && count >= limit) {
				break;
			}

			// update
			each_view = right;
		}
		return count;
	}

	template<std::invocable<HStringView> F>
	constexpr HStringView::size_type HStringView::split_each(F&& func, unicode::UTF8Seq delimiter, bool cull_empty, size_type limit) const {
		return this->split_each(
			std::forward<F>(func),
			delimiter.is_valid() ? HStringView{delimiter.data, delimiter.len} : HStringView{},
			cull_empty,
			limit
		);
	}
}

// misc
namespace hana
{
	constexpr void HStringView::swap(HStringView& v) noexcept {
		data_.swap(v.data_);
	}

	constexpr HStringView::size_type HStringView::copy(pointer dest, size_type count, size_type pos) const {
		return data_.copy(dest, count, pos);
	}
}
