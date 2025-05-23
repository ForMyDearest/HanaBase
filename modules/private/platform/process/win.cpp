#include "platform/misc.hpp"

#include <hana/log.hpp>
#include <hana/dll/shared_library.hpp>

#include <shlwapi.h>
#include <windows.h>

namespace hana
{
	struct ProcessHandle {
		PROCESS_INFORMATION processInfo = {};
		HANDLE stdOut = nullptr;
	};

	ProcessHandle* start_process(std::span<const char8_t*> args, const char8_t* stdout_file) {
		auto iter = args.begin();
		auto commandLine = HString::concat(u8"\"", *iter, u8"\"");
		while (++iter != args.end()) commandLine.append(" ").append(*iter);

		HANDLE stdOut = nullptr;
		if (stdout_file) {
			std::filesystem::path dirPath = std::filesystem::path(stdout_file).parent_path();

			if (!dirPath.empty() && !std::filesystem::exists(dirPath)) {
				if (!std::filesystem::create_directories(dirPath)) {
					LOG_ERROR(u8"Error CreateDirectories");
					return nullptr;
				}
			}

			SECURITY_ATTRIBUTES sa;
			sa.nLength = sizeof(sa);
			sa.lpSecurityDescriptor = nullptr;
			sa.bInheritHandle = TRUE;

			size_t pathLength = strlen((const char*) stdout_file) + 1;
			auto buffer = (LPWSTR) alloca(pathLength * sizeof(wchar_t));
			MultiByteToWideChar(CP_UTF8, 0, (LPCCH) stdout_file, (int) pathLength, buffer, (int) pathLength);

			stdOut = CreateFileW(buffer, FILE_APPEND_DATA, FILE_SHARE_WRITE | FILE_SHARE_READ, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		}

		STARTUPINFOA startupInfo;
		PROCESS_INFORMATION processInfo;
		memset(&startupInfo, 0, sizeof startupInfo);
		memset(&processInfo, 0, sizeof processInfo);
		startupInfo.cb = sizeof(STARTUPINFO);

		if (stdOut) {
			startupInfo.dwFlags |= STARTF_USESTDHANDLES;
			startupInfo.hStdOutput = stdOut;
			startupInfo.hStdError = stdOut;
		}

		if (!CreateProcessA(nullptr, (LPSTR) commandLine.c_str(), nullptr, nullptr, stdOut ? TRUE : FALSE, 0, nullptr, nullptr, &startupInfo, &processInfo)) {
			return nullptr;
		}

		auto* result = new ProcessHandle;
		result->processInfo = processInfo;
		result->stdOut = stdOut;
		return result;
	}

	uint64_t wait_process(const ProcessHandle* handle) noexcept {
		WaitForSingleObject(handle->processInfo.hProcess, INFINITE);
		DWORD exitCode;
		GetExitCodeProcess(handle->processInfo.hProcess, &exitCode);

		CloseHandle(handle->processInfo.hProcess);
		CloseHandle(handle->processInfo.hThread);

		if (handle->stdOut) {
			CloseHandle(handle->stdOut);
		}
		delete handle;
		return exitCode;
	}

	uint64_t get_process_id(const ProcessHandle* handle) noexcept {
		return handle->processInfo.dwProcessId;
	}

	uint64_t get_current_process_id() noexcept {
		return GetCurrentProcessId();
	}

	const char8_t* get_current_process_name() noexcept {
		DWORD fullpath_size_ = 1024;
		static char8_t fullpath_[1024] = {0};
		static char8_t pname_[64] = {0};

		const char8_t* fullpath = fullpath_;
		const char8_t* pname = pname_;

		enum {
			kNotInit      = -1,
			kInitializing = 0,
			kInitialized  = 1,
		};

		static std::atomic<int32_t> once = kNotInit;
		int32_t init = kNotInit;
		if (once.compare_exchange_strong(init, kInitializing)) {
			HANDLE handle = OpenProcess(
#if _WIN32_WINNT >= 0x0600
				PROCESS_QUERY_LIMITED_INFORMATION,
#else
				PROCESS_QUERY_INFORMATION | PROCESS_VM_READ
#endif
				FALSE, GetCurrentProcessId()
			);

			if (handle) {
				if (
#if _WIN32_WINNT >= 0x0600
					QueryFullProcessImageNameA(handle, 0, (LPSTR) fullpath_, &fullpath_size_)) {
#else
					GetModuleFileNameExA(handle, nullptr,  (LPSTR)fullpath_, fullpath_size_)) {
#endif
					SharedLibrary lib;
					lib.load(u8"Shlwapi.dll", SharedLibrary::search_system_folders);
					auto pfnPathFindFileNameA = lib.get<decltype(PathFindFileNameA)>(u8"PathFindFileNameA");
					// resolve to relative
					const auto p = pfnPathFindFileNameA((LPCSTR) fullpath); // remove path
					const auto l = strlen(p);
					const auto ll = (l > 63) ? 63 : l;
					memcpy(pname_, p, ll);
					pname_[ll] = '\0';
				} else {
					LOG_ERROR(u8"Error GetModuleBaseNameA : {}", GetLastError());
				}
				CloseHandle(handle);
			} else {
				LOG_ERROR(u8"Error OpenProcess : {}", GetLastError());
			}
			once.store(kInitialized, std::memory_order_relaxed);
		}
		while (once.load(std::memory_order_relaxed) != kInitialized) {}
		return pname ? pname : u8"unknown";
	}
}
