#pragma once

#include "hana/platform/macros.h"

#include <type_traits>

namespace hana
{
	template<typename Fn>
	class defer_raii {
	public:
		defer_raii(defer_raii&&) = delete;
		defer_raii(const defer_raii&) = delete;
		defer_raii& operator=(defer_raii&&) = delete;
		defer_raii& operator=(const defer_raii&) = delete;

		template<typename FF>
		explicit defer_raii(FF&& ff) noexcept: f(std::forward<FF>(ff)) {}

		~defer_raii() noexcept { f(); }

	private:
		Fn f;
	};

	template<typename F>
	defer_raii(F&& f) -> defer_raii<F>;
}

#define HANA_DEFER(func) hana::defer_raii HANA_UNIQUE_VARNAME(defer_object) { [&]{func;} }
