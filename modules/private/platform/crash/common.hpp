#pragma once

#include <hana/platform/macros.h>
#include <hana/platform/crash.hpp>

#include <mutex>
#include <vector>
#include <csignal>

namespace hana
{
	template<typename Derived>
	struct CrashHandle {
		struct CrashContext {
			bool continue_execution;
			CrashTerminateCode reason;
			void* usr_data;
#ifdef _WIN32
			struct _EXCEPTION_POINTERS* exception_pointers = nullptr;
#endif
		};

		struct CallbackWrapper {
			void operator()(CrashContext* context) const {
				callback(context, usr_data);
			}

			void (*callback)(CrashContext* context, void* usr_data);
			void* usr_data;
		};

		static void handle_function(CrashTerminateCode code, void* pExceptionPtrs = nullptr) {
			auto&& handle = Derived::instance();

			std::lock_guard lock(handle.crash_mutex);
			handle.ctx.reason = code;
#ifdef _WIN32
			if (pExceptionPtrs) handle.ctx.exception_pointers = (_EXCEPTION_POINTERS*) pExceptionPtrs;
#endif
			for (auto&& cb: handle.callbacks) {
				cb.callback(&handle.ctx, cb.usr_data);
			}

			handle.terminate_process(code);
		}

		static const char8_t* crash_code_string(CrashTerminateCode code) {
#define HANA_CCODE_TRANS(code) case CrashTerminateCode::code: return HANA_MAKE_UTF8(code);

			switch (code) {
				HANA_CCODE_TRANS(Abort);
				HANA_CCODE_TRANS(Interrupt);
				HANA_CCODE_TRANS(Kill);
				HANA_CCODE_TRANS(DividedByZero);
				HANA_CCODE_TRANS(IllInstruction);
				HANA_CCODE_TRANS(SegFault);
				HANA_CCODE_TRANS(StackOverflow);
				HANA_CCODE_TRANS(Terminate);
				HANA_CCODE_TRANS(Unhandled);
				HANA_CCODE_TRANS(PureVirtual);
				HANA_CCODE_TRANS(OpNewError);
				HANA_CCODE_TRANS(InvalidParam);
			}
			return u8"";
#undef HANA_CCODE_TRANS
		}

		void init() {
			static_cast<Derived*>(this)->SetProcessSignalHandlers();
			static_cast<Derived*>(this)->SetThreadSignalHandlers();
		}

		void shutdown() {
			static_cast<Derived*>(this)->UnsetThreadSignalHandlers();
			static_cast<Derived*>(this)->UnsetProcessSignalHandlers();
		}

		void SetProcessSignalHandlers() {
			using enum CrashTerminateCode;
			prev_SIGABRT = signal(SIGABRT, [](int) { handle_function(Abort); });
			prev_SIGINT = signal(SIGINT, [](int) { handle_function(Interrupt); });
			prev_SIGTERM = signal(SIGTERM, [](int) { handle_function(Kill); });
		}

		void UnsetProcessSignalHandlers() const {
			if (prev_SIGABRT) signal(SIGABRT, prev_SIGABRT);
			if (prev_SIGINT) signal(SIGINT, prev_SIGINT);
			if (prev_SIGTERM) signal(SIGTERM, prev_SIGTERM);
		}

		void SetThreadSignalHandlers() {
			using enum CrashTerminateCode;
			prev_SIGFPE = signal(SIGFPE, [](int) { handle_function(DividedByZero); });
			prev_SIGILL = signal(SIGILL, [](int) { handle_function(IllInstruction); });
			prev_SIGSEGV = signal(SIGSEGV, [](int) { handle_function(SegFault); });
		}

		void UnsetThreadSignalHandlers() const {
			if (prev_SIGFPE) signal(SIGFPE, prev_SIGFPE);
			if (prev_SIGILL) signal(SIGILL, prev_SIGILL);
			if (prev_SIGSEGV) signal(SIGSEGV, prev_SIGSEGV);
		}

		// process signal
		void (__cdecl*prev_SIGABRT)(int);
		void (__cdecl*prev_SIGINT)(int);
		void (__cdecl*prev_SIGTERM)(int);

		// thread signal
		void (__cdecl*prev_SIGFPE)(int);
		void (__cdecl*prev_SIGILL)(int);
		void (__cdecl*prev_SIGSEGV)(int);

		CrashContext ctx;
		std::vector<CallbackWrapper> callbacks;
		std::mutex crash_mutex;
	};

	inline void init_crash_handle();
	inline void shutdown_crash_handle();
}


#ifdef _WIN32
#	include "win.hpp"
#else

namespace hana
{
	struct StandardCrashHandle : CrashHandle<StandardCrashHandle> {
		static StandardCrashHandle& instance() {
			static StandardCrashHandle handle;
			return handle;
		}

		static void terminate_process(CrashTerminateCode code) {
			::exit(static_cast<int>(code));
		}
	};

	inline void init_crash_handle() {
		StandardCrashHandle::instance().init();
	}

	inline void shutdown_crash_handle() {
		StandardCrashHandle::instance().shutdown();
	}
}

#endif
