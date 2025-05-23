#pragma once

#if defined(HANA_OS_MACOSX) || defined(HANA_OS_IOS)

#include <mach-o/dyld.h>

namespace hana
{
	inline std::filesystem::path program_location_impl(std::error_code& ec) {
		ec.clear();

		char path[1024];
		uint32_t size = sizeof(path);
		if (_NSGetExecutablePath(path, &size) == 0)
			return path;

		char* p = new char[size];
		if (_NSGetExecutablePath(p, &size) != 0) {
			ec = std::make_error_code(std::errc::bad_file_descriptor);
		}

		std::filesystem::path ret(p);
		delete[] p;
		return ret;
	}
}

#elif defined(HANA_OS_LINUX) || defined(HANA_OS_ANDROID)

namespace hana
{
	inline std::filesystem::path program_location_impl(std::error_code& ec) {
		// We can not use
		// boost::dll::detail::path_from_handle(dlopen(NULL, RTLD_LAZY | RTLD_LOCAL), ignore);
		// because such code returns empty path.

		return std::filesystem::read_symlink("/proc/self/exe", ec); // Linux specific
	}
}

#endif
