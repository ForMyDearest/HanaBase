#pragma once

#include <hana/log.hpp>
#include <hana/platform/process.hpp>
#include <hana/utility/callstack.hpp>
#include <hana/dll/shared_library.hpp>

#include <new.h>
#include <windows.h>
#include <minidumpapiset.h>

namespace hana
{
	constexpr unsigned get_crash_callstack_begin() {
#ifdef NDEBUG
		return 9;
#else
		return 7;
#endif
	}

	struct WinCrashHandle : CrashHandle<WinCrashHandle> {
		using base = CrashHandle<WinCrashHandle>;

		static WinCrashHandle& instance() {
			static WinCrashHandle handle;
			return handle;
		}

		static void internal_handler(CrashContext* context) {
			if (!context->exception_pointers) context->exception_pointers = (PEXCEPTION_POINTERS) _pxcptinfoptrs;
			auto why = format(u8"Crashed! Reason: {}", crash_code_string(context->reason));
			// default max_depth?
			LOG_FATAL(u8"{} \n{}\n", why, get_callstack(get_crash_callstack_begin()));
			LogSystem::poll();

#ifndef NDEBUG
			// save crash minidump
			{
				SYSTEMTIME localTime;
				::GetLocalTime(&localTime);

				auto dateTime = format(
					u8"{}-{}-{}-{}-{}-{}-{}",
					localTime.wYear, localTime.wMonth, localTime.wDay, localTime.wHour,
					localTime.wMinute, localTime.wSecond, localTime.wMilliseconds
				);

				auto dumpPath = std::filesystem::current_path() / format(u8"{}-minidump-{}.dmp", Process::get_current_name(), dateTime).raw_data();
				HANDLE lhDumpFile = CreateFileW(dumpPath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

				MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
				loExceptionInfo.ThreadId = ::GetCurrentThreadId();
				loExceptionInfo.ClientPointers = true;
				loExceptionInfo.ExceptionPointers = context->exception_pointers;
				if (!context->exception_pointers) {
					EXCEPTION_RECORD exceptionRecord = {};
					CONTEXT contextRecord = {};

					RtlCaptureContext(&contextRecord);
					// exceptionRecord.ExceptionCode = STATUS_INVALID_PARAMETER;
					exceptionRecord.ExceptionAddress = _ReturnAddress();
					EXCEPTION_POINTERS exceptionPointers = {&exceptionRecord, &contextRecord};
					loExceptionInfo.ExceptionPointers = &exceptionPointers;
				} else {
					loExceptionInfo.ExceptionPointers = context->exception_pointers;
				}

				MiniDumpWriteDump(
					::GetCurrentProcess(),
					::GetCurrentProcessId(),
					lhDumpFile,
					MiniDumpNormal,
					&loExceptionInfo,
					nullptr,
					nullptr
				);
				::CloseHandle(lhDumpFile);
			}
#endif

			// show message box
			SharedLibrary lib;
			lib.load(u8"User32.dll", SharedLibrary::search_system_folders);
			auto pfnMessageBoxExA = lib.get<decltype(MessageBoxExA)>(u8"MessageBoxExA");
			pfnMessageBoxExA(nullptr, why.raw_data(), "Crash happened!", MB_ABORTRETRYIGNORE | MB_ICONERROR, 0);
		}

		static void terminate_process(CrashTerminateCode code) {
			TerminateProcess(GetCurrentProcess(), static_cast<UINT>(code));
		}

		void init() {
			base::init();

			// The following code is intended to fix the issue with 32-bit applications in 64-bit environment.
			// http://support.microsoft.com/kb/976038/en-us
			// http://code.google.com/p/crashrpt/issues/detail?id=104
			typedef BOOL (WINAPI *SETPROCESSUSERMODEEXCEPTIONPOLICY)(DWORD dwFlags);
			typedef BOOL (WINAPI *GETPROCESSUSERMODEEXCEPTIONPOLICY)(LPDWORD lpFlags);
#define PROCESS_CALLBACK_FILTER_ENABLED 0x1
			if (const HMODULE hKernel32 = LoadLibraryA("kernel32.dll")) {
				auto pfnSetProcessUserModeExceptionPolicy = (SETPROCESSUSERMODEEXCEPTIONPOLICY) GetProcAddress(hKernel32, "SetProcessUserModeExceptionPolicy");
				auto pfnGetProcessUserModeExceptionPolicy = (GETPROCESSUSERMODEEXCEPTIONPOLICY) GetProcAddress(hKernel32, "GetProcessUserModeExceptionPolicy");
				if (pfnSetProcessUserModeExceptionPolicy && pfnGetProcessUserModeExceptionPolicy) {
					DWORD dwFlags = 0;
					if (pfnGetProcessUserModeExceptionPolicy(&dwFlags)) {
						pfnSetProcessUserModeExceptionPolicy(dwFlags & ~PROCESS_CALLBACK_FILTER_ENABLED);
					}
				}
				FreeLibrary(hKernel32);
			}

			callbacks.emplace_back([](CrashContext* context, void*) {
				internal_handler(context);
			}, this);
		}

		void SetProcessSignalHandlers() {
			using enum CrashTerminateCode;

			prev_exception_filter = ::SetUnhandledExceptionFilter([](PEXCEPTION_POINTERS pExceptionPtrs)-> LONG {
				// Handle stack overflow in a separate thread.
				// Vojtech: Based on martin.bis...@gmail.com comment in
				// http://groups.google.com/group/crashrpt/browse_thread/thread/a1dbcc56acb58b27/fbd0151dd8e26daf?lnk=gst&q=stack+overflow#fbd0151dd8e26daf
				const auto hasExceptionRecord = pExceptionPtrs && pExceptionPtrs->ExceptionRecord;

				// stack overflow
				if (hasExceptionRecord && pExceptionPtrs->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW) {
					// Special case to handle the stack overflow exception.
					// The dump will be realized from another thread.
					// Create another thread that will do the dump.
					HANDLE thread = ::CreateThread(
						nullptr,
						0,
						[](LPVOID lpParameter)-> DWORD { return handle_function(StackOverflow, lpParameter), 0; },
						pExceptionPtrs,
						0,
						nullptr
					);
					::WaitForSingleObject(thread, INFINITE);
					::CloseHandle(thread);

					handle_function(Unhandled, pExceptionPtrs);
				}

				return EXCEPTION_EXECUTE_HANDLER;
			});

			_set_error_mode(_OUT_TO_STDERR);

			// Catch pure virtual function calls.
			// Because there is one _purecall_handler for the whole process,
			// calling this function immediately impacts all threads. The last caller on any thread sets the handler.
			// http://msdn.microsoft.com/en-us/library/t296ys27.aspx
			prev_purec = _set_purecall_handler([] { handle_function(PureVirtual); });

			// Catch new operator memory allocation exceptions
			_set_new_mode(1); // Force malloc() to call new handler too
			prev_new_handler = std::set_new_handler([] {
				handle_function(OpNewError);
			});

			// Catch invalid parameter exceptions.
			prev_invpar = _set_invalid_parameter_handler([](const wchar_t*, const wchar_t*, const wchar_t*, unsigned int, uintptr_t) {
				handle_function(InvalidParam);
			});

			_set_abort_behavior(_CALL_REPORTFAULT, _CALL_REPORTFAULT);

			base::SetProcessSignalHandlers();
		}

		void UnsetProcessSignalHandlers() {
			if (prev_exception_filter) SetUnhandledExceptionFilter(prev_exception_filter);
			if (prev_purec) _set_purecall_handler(prev_purec);
			if (prev_new_handler) std::set_new_handler(prev_new_handler);
			if (prev_invpar) _set_invalid_parameter_handler(prev_invpar);

			base::UnsetThreadSignalHandlers();
		}

		void SetThreadSignalHandlers() {
			using enum CrashTerminateCode;
			prev_term = set_terminate([] { handle_function(Terminate); });
			base::SetThreadSignalHandlers();
		}

		void UnsetThreadSignalHandlers() {
			if (prev_SIGFPE) signal(SIGFPE, prev_SIGFPE);

			base::UnsetProcessSignalHandlers();
		}

		// process signal
		LPTOP_LEVEL_EXCEPTION_FILTER prev_exception_filter = nullptr;
		_purecall_handler prev_purec = nullptr;
		std::new_handler prev_new_handler = nullptr;
		_invalid_parameter_handler prev_invpar = nullptr;

		// thread signal
		terminate_handler prev_term = nullptr;
	};

	inline void init_crash_handle() {
		WinCrashHandle::instance().init();
	}

	inline void shutdown_crash_handle() {
		WinCrashHandle::instance().shutdown();
	}
}
