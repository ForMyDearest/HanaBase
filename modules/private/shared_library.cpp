#include "misc.hpp"

#include <hana/shared_library.hpp>

#ifdef _WIN32

namespace hana
{
	const char8_t* suffix() {
		return u8".dll";
	}

	std::string get_last_error() {
		return std::error_code(static_cast<int>(GetLastError()), std::system_category()).message();
	}

	std::filesystem::path decorate(const std::filesystem::path& sl) {
		std::filesystem::path actual_path = sl;
		actual_path += suffix();
		return actual_path;
	}
}

namespace hana
{
	bool SharedLibrary::load(const char8_t* lib_path, LoadMode mode) noexcept {
		unload();
		if (!lib_path) {
			handle_ = GetModuleHandle(nullptr);
			return handle_;
		}

		auto native_mode = static_cast<DWORD>(mode);

		std::filesystem::path sl = lib_path;
		if (!sl.is_absolute() && !(native_mode & search_system_folders)) {
			std::error_code current_path_ec;
			std::filesystem::path prog_loc = std::filesystem::current_path(current_path_ec);

			if (!current_path_ec) {
				prog_loc /= sl;
				sl.swap(prog_loc);
			}
		}
		native_mode = static_cast<unsigned>(native_mode) & ~static_cast<unsigned>(search_system_folders);

		// Trying to open with appended decorations
		if (native_mode & append_decorations) {
			native_mode = static_cast<unsigned>(native_mode) & ~static_cast<unsigned>(append_decorations);

			auto load_path = decorate(sl);
			handle_ = LoadLibraryExW(load_path.c_str(), nullptr, native_mode);
			if (handle_) {
				return true;
			}
			if (std::filesystem::exists(load_path)) {
				// decorated path exists : current error is not a bad file descriptor
				return false;
			}

			// MinGW loves 'lib' prefix and puts it even on Windows platform.
			std::filesystem::path mingw_load_path = (sl.has_parent_path() ? sl.parent_path() / L"lib" : L"lib").native();
			mingw_load_path += sl.filename().native();
			mingw_load_path += suffix();

			handle_ = LoadLibraryExW(mingw_load_path.c_str(), nullptr, native_mode);
			if (handle_) {
				return true;
			}
			if (std::filesystem::exists(mingw_load_path)) {
				// decorated path exists : current error is not a bad file descriptor
				return false;
			}
		}

		// From MSDN: If the string specifies a module name without a path and the
		// file name extension is omitted, the function appends the default library
		// extension .dll to the module name.
		//
		// From experiments: Default library extension appended to the module name even if
		// we have some path. So we do not check for path, only for extension. We can not be sure that
		// such behavior remain across all platforms, so we add L"." by hand.

		if (sl.has_extension()) {
			handle_ = LoadLibraryExW(sl.c_str(), nullptr, native_mode);
		} else {
			handle_ = LoadLibraryExW((sl.native() + L".").c_str(), nullptr, native_mode);
		}

		// LoadLibraryExW method is capable of self loading from program_location() path. No special actions
		// must be taken to allow self loading.
		return handle_;
	}

	bool SharedLibrary::unload() noexcept {
		if (handle_ && FreeLibrary(handle_)) {
			handle_ = nullptr;
			return true;
		}
		return false;
	}

	bool SharedLibrary::has_symbol(const char8_t* symbol_name) const noexcept {
		return is_loaded() && GetProcAddress(handle_, reinterpret_cast<const char*>(symbol_name));
	}

	void* SharedLibrary::get_symbol(const char8_t* symbol_name) const noexcept {
		if (!is_loaded()) {
			return nullptr;
		}
		return GetProcAddress(handle_, reinterpret_cast<const char*>(symbol_name));
	}

	std::filesystem::path SharedLibrary::get_location() const noexcept {
		if (!is_loaded()) {
			return {};
		}

		constexpr DWORD DEFAULT_PATH_SIZE = 260;

		// If `handle` parameter is NULL, GetModuleFileName retrieves the path of the
		// executable file of the current process.
		WCHAR path_hldr[DEFAULT_PATH_SIZE];
		if (GetModuleFileNameW(handle_, path_hldr, DEFAULT_PATH_SIZE)) {
			// On success, GetModuleFileNameW() doesn't reset last error to ERROR_SUCCESS. Resetting it manually.
			return path_hldr;
		}

		auto ec = GetLastError();
		for (unsigned i = 2; i < 1025 && ec == ERROR_INSUFFICIENT_BUFFER; i *= 2, ec = GetLastError()) {
			std::wstring p(DEFAULT_PATH_SIZE * i, L'\0');
			if (const auto size = GetModuleFileNameW(handle_, &p[0], DEFAULT_PATH_SIZE * i); size) {
				// On success, GetModuleFileNameW() doesn't reset last error to ERROR_SUCCESS. Resetting it manually.
				p.resize(size);
				return p;
			}
		}
		// Error other than ERROR_INSUFFICIENT_BUFFER_ occurred or failed to allocate buffer big enough.
		return {};
	}
}

#else

#pragma region program_location_impl

#if defined(OS_MACOSX) || defined(OS_IOS)

#include <mach-o/dyld.h>

namespace boost
{
	std::filesystem::path program_location_impl(std::error_code& ec) {
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

#elif defined(OS_LINUX) || defined(OS_ANDROID)

namespace boost
{
	std::filesystem::path program_location_impl(std::error_code& ec) {
		return std::filesystem::read_symlink("/proc/self/exe", ec); // Linux specific
	}
}

#endif

#pragma endregion program_location_impl

#pragma region SharedLibrary

#if !defined(OS_MACOSX) && !defined(OS_IOS)
#   include <link.h>
#endif

namespace hana
{
	const char8_t* suffix() {
		// https://sourceforge.net/p/predef/wiki/OperatingSystems/
#if defined(OS_MACOSX) || defined(OS_IOS)
		return u8".dylib";
#else
		return u8".so";
#endif
	}

	std::string get_last_error() {
		return static_cast<const char*>(dlerror());
	}

	std::filesystem::path decorate(const std::filesystem::path& sl) {
		std::filesystem::path actual_path;
		if (std::strncmp(sl.filename().string().c_str(), "lib", 3) != 0) {
			auto s1 = sl.has_parent_path() ? sl.parent_path() / "lib" : "lib";
			actual_path = s1.native() + sl.filename().native();
		} else {
			actual_path = sl;
		}

		actual_path += suffix();
		return actual_path;
	}

	bool SharedLibrary::load(const char8_t* lib_path, LoadMode mode) noexcept {
		auto native_mode = static_cast<int>(mode);
		unload();

		// Do not allow opening NULL paths. User must use program_location() instead
		if (!lib_path) {
			return false;
		}

		std::filesystem::path sl = lib_path;

		// Fixing modes

		if (!(native_mode & rtld_now)) {
			native_mode |= rtld_lazy;
		}

		if (!(native_mode & rtld_global)) {
			native_mode |= rtld_local;
		}

#if defined(OS_LINUX) || defined(OS_ANDROID)
		if (!sl.has_parent_path() && !(native_mode & transLoadMode(search_system_folders))) {
            sl = "." / sl;
        }
#else
		if (!sl.is_absolute() && !(native_mode & search_system_folders)) {
			std::error_code current_path_ec;
			std::filesystem::path prog_loc = std::filesystem::current_path(current_path_ec);
			if (!current_path_ec) {
				prog_loc /= sl;
				sl.swap(prog_loc);
			}
		}
#endif

		native_mode = static_cast<unsigned>(native_mode) & ~static_cast<unsigned>(search_system_folders);

		// Trying to open with appended decorations

		if (native_mode & append_decorations) {
			native_mode = static_cast<unsigned>(native_mode) & ~static_cast<unsigned>(append_decorations);

			auto actual_path = decorate(sl);
			handle_ = dlopen(actual_path.c_str(), native_mode);
			if (handle_) {
				return true;
			}
			std::error_code prog_loc_err;
			std::filesystem::path loc = boost::program_location_impl(prog_loc_err);
			if (std::filesystem::exists(actual_path, prog_loc_err) && !std::filesystem::equivalent(sl, loc, prog_loc_err)) {
				// decorated path exists : current error is not a bad file descriptor and we are not trying to load the executable itself
				return false;
			}
		}

		// Opening by exactly specified path
		handle_ = dlopen(sl.c_str(), native_mode);
		if (handle_) {
			return true;
		}

		// Maybe user wanted to load the executable itself? Checking...
		// We assume that usually user wants to load a dynamic library not the executable itself, that's why
		// we try this only after traditional load fails.
		std::error_code prog_loc_err;
		std::filesystem::path loc = boost::program_location_impl(prog_loc_err);
		if (!prog_loc_err && std::filesystem::equivalent(sl, loc, prog_loc_err) && !prog_loc_err) {
			// As is known the function dlopen() loads the dynamic library file
			// named by the null-terminated string filename and returns an opaque
			// "handle" for the dynamic library. If filename is NULL, then the
			// returned handle is for the main program.
			handle_ = dlopen(nullptr, native_mode);
		}
		return handle_;
	}

	bool SharedLibrary::unload() noexcept {
		if (handle_ && dlclose(handle_) != 0) {
			handle_ = nullptr;
			return true;
		}
		return false;
	}

	bool SharedLibrary::has_symbol(const char8_t* symbol_name) const noexcept {
		return is_loaded() && dlsym(handle_, reinterpret_cast<const char*>(symbol_name));
	}

	void* SharedLibrary::get_symbol(const char8_t* symbol_name) const noexcept {
		if (!is_loaded()) {
			return nullptr;
		}
		return dlsym(handle_, reinterpret_cast<const char*>(symbol_name));
	}

	std::filesystem::path SharedLibrary::get_location() const noexcept {
		if (!is_loaded()) {
			return {};
		}

		const struct link_map* link_map = nullptr;
		link_map = static_cast<const struct link_map*>(handle_);
		if (!link_map) {
			return {};
		}

		if (!link_map->l_name || *link_map->l_name == '\0') {
			std::error_code ec;
			return boost::program_location_impl(ec);
		}

		return link_map->l_name;
	}
}

#pragma endregion SharedLibrary

#endif
