#pragma once

#include <type_traits>

namespace hana
{
	namespace internal
	{
		template<typename T, typename Elem>
		concept HasAppend = requires(T t, Elem e)
		{
			t.append(e);
		};
		template<typename T, typename Elem>
		concept HasAdd = requires(T t, Elem e)
		{
			t.add(e);
		};
		template<typename T, typename Elem>
		concept HasEmplaceBack = requires(T t, Elem e)
		{
			t.emplace_back(e);
		};
		template<typename T, typename Elem>
		concept HasPushBack = requires(T t, Elem e)
		{
			t.push_back(e);
		};

		template<typename T, typename Elem>
		concept CanAdd = HasAppend<T, Elem> || HasAdd<T, Elem> || HasEmplaceBack<T, Elem> || HasPushBack<T, Elem>;
	}

	// =========================================================

	template<typename, template <typename...> typename>
	struct is_specialization : std::false_type {};

	template<template <typename...> typename Template, typename... Ts>
	struct is_specialization<Template<Ts...>, Template> : std::true_type {};

	template<typename T, template <typename...> typename Template>
	concept is_specialization_v = is_specialization<T, Template>::value;

	// =========================================================

	template<typename T>
	concept is_ebco_eligible_v = std::is_empty_v<T> && !std::is_final_v<T>;

	template<typename T, typename... Ts>
	inline constexpr bool is_any_of_v = std::disjunction_v<std::is_same<T, Ts>...>;

	template<typename T>
	concept is_char_v = is_any_of_v<T, char, char8_t, char16_t, char32_t, wchar_t>;

	template<typename T>
	concept is_transparent_v = requires { typename T::is_transparent; };

	template<typename T>
	concept is_complete_v = (sizeof(T) != 0);

	// =========================================================

	template<typename, typename = void>
	struct has_iterator_category : std::false_type {};

	template<typename T>
	struct has_iterator_category<T, std::void_t<typename std::iterator_traits<T>::iterator_category>> : std::true_type {};

	template<typename T, typename = void>
	struct is_iterator : std::false_type {};

	template<typename T>
	struct is_iterator<T, std::enable_if_t<!std::is_void_v<std::remove_cv_t<std::remove_pointer_t<T>>>>> : has_iterator_category<T> {};

	template<typename T>
	concept is_iterator_v = is_iterator<T>::value;
}
