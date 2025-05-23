#pragma once

#include "hana/platform/macros.h"

#include <span>

namespace hana
{
	HANA_BASE_API struct ProcessHandle* start_process(std::span<const char8_t*> args, const char8_t* stdout_file);
	HANA_BASE_API uint64_t wait_process(const ProcessHandle* handle) noexcept;
	HANA_BASE_API uint64_t get_process_id(const ProcessHandle* handle) noexcept;

	HANA_BASE_API uint64_t get_current_process_id() noexcept;
	HANA_BASE_API const char8_t* get_current_process_name() noexcept;
}
