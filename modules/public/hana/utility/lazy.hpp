#pragma once

#include <optional>

namespace hana
{
	template<typename T>
	class lazy_t {
	public:
		constexpr lazy_t() noexcept = default;

		constexpr lazy_t(lazy_t&&) = delete;
		constexpr lazy_t(const lazy_t&) = delete;
		constexpr lazy_t& operator=(lazy_t&&) = delete;
		constexpr lazy_t& operator=(const lazy_t&) = delete;

		constexpr explicit operator T() = delete;
		constexpr explicit operator bool() const noexcept { return data_.has_value(); }
		[[nodiscard]] constexpr bool is_initialized() const noexcept { return data_.has_value(); }

		template<typename... Args> requires(std::is_constructible_v<T, Args...>)
		constexpr void reset(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
			data_.emplace(std::forward<Args>(args)...);
		}

		template<typename... Args> requires(std::is_constructible_v<T, Args...>)
		constexpr T& get(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
			if (!data_.has_value()) {
				data_.emplace(std::forward<Args>(args)...);
			}
			return data_.value();
		}

		template<typename... Args> requires(std::is_constructible_v<T, Args...>)
		constexpr const T& get(Args&&... args) const noexcept(std::is_nothrow_constructible_v<T, Args...>) {
			if (!data_.has_value()) {
				data_.emplace(std::forward<Args>(args)...);
			}
			return data_.value();
		}

	private:
		mutable std::optional<T> data_;
	};
}
