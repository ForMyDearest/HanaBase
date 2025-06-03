#pragma once

#include "hana/platform/macros.h"
#include "hana/container/string_view.hpp"

namespace hana
{
	class HANA_BASE_API Thread {
	public:
		static constexpr int MAX_THREAD_NAME_LENGTH = 31;

		enum Priority : uint8_t {
			DEFAULT,
			LOWEST,
			BELOW_NORMAL,
			NORMAL,
			ABOVE_NORMAL,
			HIGH,
			TIME_CRITICAL
		};

		struct Description {
			void (*func)(void*);
			void* ctx;
		};

		static Thread* init(Description desc) noexcept;
		static Thread* get_current_thread() noexcept;
		static void set_current_name(HStringView name) noexcept;
		static const char8_t* get_current_name() noexcept;

		Priority set_priority(Priority pr) noexcept;
		void set_affinity(size_t affinity_mask) noexcept;
		void join() noexcept;
		void destroy() noexcept;

	private:
		Thread() = default;
		~Thread() = default;
	};
}
