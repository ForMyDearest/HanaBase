#pragma once

/*

MIT License

Copyright (c) 2019 mjwatkins2

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <atomic>
#include <cassert>

namespace hana
{
	/*!
	 * @brief
	 *		Provides similar capability as std::shared_mutex but does so with "lock-free programming" atomic operations.
	 *		Also provides an atomic shared-to-unique-lock upgrade capability, which std::shared_mutex does not provide.
	 *
	 *		This can be used with std::shared_lock, std::unique_lock, etc. but is best used with the (boost::)upgradable_lock.
	 *
	 *		This mutex works by keeping track of the number of threads with a shared lock and the number of threads waiting for a unique lock
	 *		(one of which may be uniquely locked). Unique locks have priority: all threads asking for a shared lock are blocked until the
	 *		number of threads waiting for a unique lock is zero. All threads asking for a unique lock are blocked until the number of threads
	 *		with a shared lock is zero, and also until there is no active unique lock (each thread asking for a unique lock proceeds one-at-a-time).
	 *
	 *		To guarantee atomicity of all shared/unique thread counting, updating, and checking, all data is stored in one atomic integer using
	 *		bit masking to count the number of threads *with* shared locks, the number of the threads *waiting* for a unique lock, and a flag
	 *		indicating whether there is an active unique lock.
	 *
	 *		The order of thread unblocking is not guaranteed. All threads just spin in loops until unblocked.
	 *
	 * @code
	 *		shared_atomic_mutex m;
	 *		m.lock_shared();
	 *		<== read stuff from a cache on this line ==>
	 *		if (*need to write to the cache*) {
	 *			m.upgrade();
	 *			<== write stuff to cache ==>
	 *			m.unlock();
	 *		}
	 *		else {
	 *			m.unlock_shared();
	 *		}
	 * @endcode
	 *
	 * @note
	 *		[1] Do call unlock_shared() when a shared lock is no longer necessary. You must explicitly unlock. \n
	 *		[2] Do call unlock() when a unique lock is no longer necessary. You must explicitly unlock. \n
	 *		[3] Do not acquire a unique lock before unlocking any shared lock in the same thread, unless upgrading. \n
	 *		[4] Do not acquire a shared lock before unlocking any unique lock in the same thread. \n
	 */
	class shared_atomic_mutex {
	public:
		shared_atomic_mutex() noexcept = default;
		shared_atomic_mutex(const shared_atomic_mutex&) = delete;
		shared_atomic_mutex& operator=(const shared_atomic_mutex&) = delete;

		// Acquire the unique lock, first waiting until all shared locks are unlocked.
		void lock() noexcept {
			// Increment the unique lock count (which is the number of threads waiting for a unique lock).
			// Incrementing immediately, whether actively uniquely locked or not, blocks any new thread asking for a new shared lock.
			m_bitfield += one_unique_thread;
			acquire_unique();
		}

		// Unlock the unique lock.
		void unlock() noexcept {
			assert(num_unique_locks() > 0 && is_unique_locked());
			m_bitfield -= (one_unique_flag + one_unique_thread);
		}

		// Upgrade a previously-acquired shared lock to the unique lock.
		void upgrade() noexcept {
			static_assert(one_unique_thread > one_shared_thread, "This section of code assumes one_unique_thread > one_shared_thread for subtraction");
			// increment the unique lock count and remove this thread from the shared lock thread count
			m_bitfield += (one_unique_thread - one_shared_thread);
			acquire_unique();
		}

		// Acquire a shared lock, first waiting until all unique locks are unlocked.
		void lock_shared() noexcept {
			// Unlike the unique lock, don't immediately increment the shared count (wait until there are no unique lock requests before adding).
			uint32_t oldval = m_bitfield;
			uint32_t newval;
			do {
				// Proceed if any number of shared locks and no unique locks are waiting or active.
				any_shared_no_unique(oldval);
				// New value is expected value with a new shared lock (increment the shared lock count).
				newval = oldval + one_shared_thread;

				// If m_bitfield == oldval (there are no unique locks) then store newval in m_bitfield (add a shared lock).
				// Otherwise update oldval with the latest value of m_bitfield and run the test loop again.
			} while (!m_bitfield.compare_exchange_weak(oldval, newval, std::memory_order::memory_order_relaxed));
		}

		// Unlock a shared lock.
		void unlock_shared() noexcept {
			assert(num_shared_locks() > 0);
			m_bitfield -= one_shared_thread;
		}

		// Returns the number of threads with an active shared lock.
		uint16_t num_shared_locks() const noexcept {
			return (m_bitfield & num_shared_mask) >> num_shared_bitshift;
		}

		// Returns the number of threads waiting for a unique lock, one of which may have an active unique lock.
		uint16_t num_unique_locks() const noexcept {
			return (m_bitfield & num_unique_mask) >> num_unique_bitshift;
		}

		// Returns whether the mutex is currently uniquely locked.
		bool is_unique_locked() const noexcept {
			return (m_bitfield & unique_flag_mask) >> unique_flag_bitshift;
		}

	private:
		void acquire_unique() noexcept {
			uint32_t oldval = m_bitfield;
			uint32_t newval;
			do {
				// Proceed if there are no shared locks and the unique lock is available (unique lock flag is 0).
				no_shared_no_unique(oldval);
				// Set the unique lock flag to 1.
				newval = oldval + one_unique_flag;

				// If m_bitfield == oldval (there are no active shared locks and no thread has a unique lock) then store newval in m_bitfield (get the unique lock).
				// Otherwise update oldval with the latest value of m_bitfield and run the test loop again.
			} while (!m_bitfield.compare_exchange_weak(oldval, newval, std::memory_order::memory_order_relaxed));
		}

		static constexpr uint8_t num_shared_bitshift = 0;
		static constexpr uint8_t num_unique_bitshift = 16;
		static constexpr uint8_t unique_flag_bitshift = 31;

		static constexpr uint32_t num_shared_mask = 65535 << num_shared_bitshift;
		static constexpr uint32_t num_unique_mask = 32767 << num_unique_bitshift;
		static constexpr uint32_t unique_flag_mask = 1 << unique_flag_bitshift;

		static constexpr uint32_t one_shared_thread = 1 << num_shared_bitshift;
		static constexpr uint32_t one_unique_thread = 1 << num_unique_bitshift;
		static constexpr uint32_t one_unique_flag = 1 << unique_flag_bitshift;

		static constexpr void any_shared_no_unique(uint32_t& bits) noexcept { bits &= num_shared_mask; }
		static constexpr void no_shared_no_unique(uint32_t& bits) noexcept { bits &= num_unique_mask; }

		std::atomic<uint32_t> m_bitfield = 0; // consists of [ 1 bit unique lock flag, 15 bits # of unique locks, 16 bits # of shared locks]
	};
}
