#pragma once

#ifdef _MSC_VER
#	include <intrin.h>
#endif

#include <thread>

namespace hana
{
	struct spinlock {
		std::atomic_flag flag_;

		bool try_lock() noexcept {
			return !flag_.test_and_set(std::memory_order_acquire);
		}

		void lock() noexcept {
			for (unsigned k = 0; !try_lock(); ++k) {
				if (k & 1) {
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				} else {
					spin_loop_pause();
				}
			}
		}

		void unlock() noexcept {
			flag_.clear(std::memory_order_release);
		}

		static void spin_loop_pause() noexcept {
#if defined(__has_builtin) && __has_builtin(__builtin_ia32_pause) && !defined(__INTEL_COMPILER)
			__builtin_ia32_pause();
#elif defined(_MSC_VER) && ( defined(_M_IX86) || defined(_M_X64) )
			_mm_pause();
#elif defined(_MSC_VER) && ( defined(_M_ARM) || defined(_M_ARM64) )
			__yield();
#elif defined(__GNUC__) && ( defined(__i386__) || defined(__x86_64__) )
			__asm__ __volatile__( "rep; nop" : : : "memory" );
#elif defined(__GNUC__) && ( (defined(__ARM_ARCH) && __ARM_ARCH >= 8) || defined(__ARM_ARCH_8A__) || defined(__aarch64__) )
			__asm__ __volatile__( "yield" : : : "memory" );
#endif
		}
	};
}
