#pragma once

#include "hana/memory/rc.hpp"
#include "hana/platform/macros.h"
#include "hana/container/string_view.hpp"

namespace hana
{
	enum class ThreadPriority : uint8_t {
		DEFAULT,
		LOWEST,
		BELOW_NORMAL,
		NORMAL,
		ABOVE_NORMAL,
		HIGH,
		TIME_CRITICAL
	};

	class HANA_BASE_API Thread : RCUniqueInterface {
	public:
		static constexpr int MAX_THREAD_NAME_LENGTH = 31;

		struct Description {
			void (*func)(void*);
			void* ctx;
		};

		static RCUnique<Thread> init(Description desc) noexcept;
		static Thread* get_current_thread() noexcept;
		static void set_current_name(HStringView name) noexcept;
		static const char8_t* get_current_name() noexcept;

		ThreadPriority set_priority(ThreadPriority pr) noexcept;
		void set_affinity(size_t affinity_mask) noexcept;
		void join() noexcept;

	private:
		template<typename> friend class RCUnique;
		void rc_delete() noexcept;

		Thread() = default;
		~Thread() = default;
	};
}
