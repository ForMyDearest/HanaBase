#include <hana/log.hpp>
#include <hana/platform/process.hpp>

#include <vector>
#include <csignal>

#include <unistd.h>
#include <spawn.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <process.h>

namespace hana
{
	struct ProcessHandle {
		pid_t pid;
	};

	Process* Process::start(std::span<const char8_t*> args, const char8_t* stdout_file) noexcept {
		// int errcode = system(cmd.c_str()); (void)errcode;
		extern char** environ; // provided by libc
		pid_t ChildPid = -1;

		posix_spawnattr_t SpawnAttr;
		posix_spawnattr_init(&SpawnAttr);
		short int SpawnFlags = 0;

		// unmask all signals and set realtime signals to default for children
		// the latter is particularly important for mono, which otherwise will crash attempting to find usable signals
		// (NOTE: setting all signals to default fails)
		sigset_t EmptySignalSet;
		sigemptyset(&EmptySignalSet);
		posix_spawnattr_setsigmask(&SpawnAttr, &EmptySignalSet);
		SpawnFlags |= POSIX_SPAWN_SETSIGMASK;

		sigset_t SetToDefaultSignalSet;
		sigemptyset(&SetToDefaultSignalSet);

		long SigrtMin, SigrtMax;
#if defined(SIGRTMIN) && defined(SIGRTMAX)
    SigrtMin = SIGRTMIN;
    SigrtMax = SIGRTMAX;
#elif defined(_SC_SIGRT_MIN) && defined(_SC_SIGRT_MAX)
    SigrtMin = sysconf(_SC_SIGRT_MIN);
    SigrtMax = sysconf(_SC_SIGRT_MAX);
#else
		SigrtMin = SIGUSR1;
		SigrtMax = SIGUSR2;
#endif
		for (int SigNum = SigrtMin; SigNum <= SigrtMax; ++SigNum) {
			sigaddset(&SetToDefaultSignalSet, SigNum);
		}

		posix_spawnattr_setsigdefault(&SpawnAttr, &SetToDefaultSignalSet);
		SpawnFlags |= POSIX_SPAWN_SETSIGDEF;

		// Makes spawned processes have its own unique group id so we can kill the entire group with out killing the parent
		SpawnFlags |= POSIX_SPAWN_SETPGROUP;

		int PosixSpawnErrNo = -1; {
#ifdef POSIX_SPAWN_USEVFORK
        // if we don't have any actions to do, use a faster route that will use vfork() instead.
        // This is not just faster, it is crucial when spawning a crash reporter to report a crash due to stack overflow in a thread
        // since otherwise atfork handlers will get called and posix_spawn() will crash (in glibc's __reclaim_stacks()).
        // However, it has its problems, see:
        //		http://ewontfix.com/7/
        //		https://sourceware.org/bugzilla/show_bug.cgi?id=14750
        //		https://sourceware.org/bugzilla/show_bug.cgi?id=14749
        SpawnFlags |= POSIX_SPAWN_USEVFORK;
#endif

			posix_spawnattr_setflags(&SpawnAttr, SpawnFlags);
			PosixSpawnErrNo = posix_spawn(&ChildPid, args[0], nullptr, &SpawnAttr, &args[1], environ);
		}
		posix_spawnattr_destroy(&SpawnAttr);

		if (PosixSpawnErrNo != 0) {
			LOG_ERROR(u8"hana_run_process: posix_spawn() failed ({}, {})", PosixSpawnErrNo, strerror(PosixSpawnErrNo));
			return nullptr;
		}

		auto* result = new ProcessHandle;
		result->pid = ChildPid;
		return reinterpret_cast<Process*>(result);
	}

	uint64_t Process::wait() noexcept {
		auto* handle = reinterpret_cast<ProcessHandle*>(this);
		const auto pid = handle->pid;
		delete handle;
		int status;
		if (waitpid(pid, &status, WNOHANG) == -1) {
			LOG_ERROR(u8"wait() error");
		} else {
			if (WIFEXITED(status)) return 0;
			return -1;
		}
		return -1;
	}

	uint64_t Process::get_pid() const noexcept {
		return reinterpret_cast<const ProcessHandle*>(this)->pid;
	}

	uint64_t Process::get_current_pid() noexcept {
		return _getpid();
	}

	const char8_t* Process::get_current_name() noexcept {
#if defined(__APPLE__) || defined(__FreeBSD__)
		return (const char8_t*)getprogname();
#elif defined(_GNU_SOURCE)
		return (const char8_t*) program_invocation_name;
#else
		return u8"?";
#endif
	}
}
