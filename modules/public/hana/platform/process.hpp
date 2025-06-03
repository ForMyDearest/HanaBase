#pragma once

#include "hana/platform/macros.h"

#include <span>

namespace hana
{
	class HANA_BASE_API Process {
	public:
		static Process* start(std::span<const char8_t*> args, const char8_t* stdout_file) noexcept;
		static uint64_t get_current_pid() noexcept;
		static const char8_t* get_current_name() noexcept;

		uint64_t wait() noexcept;
		uint64_t get_pid() const noexcept;

	private:
		Process() = default;
		~Process() = default;
	};
}
