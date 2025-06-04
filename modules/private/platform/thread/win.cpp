#include <hana/platform/thread.hpp>

#include <iterator>

#include <windows.h>
#include <process.h>

namespace hana
{
	thread_local char thread_name[Thread::MAX_THREAD_NAME_LENGTH + 1] = "unnamed";

	static constexpr int priorities[] = {
		THREAD_PRIORITY_NORMAL,
		THREAD_PRIORITY_LOWEST,
		THREAD_PRIORITY_BELOW_NORMAL,
		THREAD_PRIORITY_NORMAL,
		THREAD_PRIORITY_ABOVE_NORMAL,
		THREAD_PRIORITY_HIGHEST,
		THREAD_PRIORITY_TIME_CRITICAL
	};

	unsigned WINAPI ThreadFunctionStatic(void* data) {
		const auto* desc = static_cast<Thread::Description*>(data);
		desc->func(desc->ctx);
		return 0;
	}

	Thread* Thread::init(Description desc) noexcept {
		const auto handle = _beginthreadex(nullptr, 0, &ThreadFunctionStatic, &desc, 0, nullptr);
		assert(handle);
		return reinterpret_cast<Thread*>(handle);
	}

	Thread* Thread::get_current_thread() noexcept {
		return static_cast<Thread*>(GetCurrentThread());
	}

	void Thread::set_current_name(HStringView name) noexcept {
		auto* buffer = new wchar_t[name.size() + 1];
		size_t resultLength = MultiByteToWideChar(CP_UTF8, 0, (LPCCH) name.raw_data(), name.size(), buffer, name.size());
		buffer[resultLength] = 0;

		SetThreadDescription(GetCurrentThread(), buffer);
		strcpy_s(thread_name, MAX_THREAD_NAME_LENGTH + 1, name.raw_data());
		delete buffer;
	}

	const char8_t* Thread::get_current_name() noexcept {
		return reinterpret_cast<const char8_t*>(thread_name);
	}

	ThreadPriority Thread::set_priority(ThreadPriority pr) noexcept {
		return SetThreadPriority(this, priorities[static_cast<int>(pr)]) ? pr : static_cast<ThreadPriority>(GetThreadPriority(this));
	}

	void Thread::set_affinity(size_t affinity_mask) noexcept {
		SetThreadAffinityMask(this, affinity_mask);
	}

	void Thread::join() noexcept {
		WaitForSingleObject(this, INFINITE);
	}

	void Thread::destroy() noexcept {
		join();
		CloseHandle(this);
	}
}
