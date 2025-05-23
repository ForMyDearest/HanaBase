#include "program_location_impl.hpp"

#if !defined(HANA_OS_MACOSX) && !defined(HANA_OS_IOS)
#   include <link.h>
#endif

namespace hana
{
	const char8_t* suffix() {
		// https://sourceforge.net/p/predef/wiki/OperatingSystems/
#if defined(HANA_OS_MACOSX) || defined(HANA_OS_IOS)
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
		int native_mode = mode;
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

#if defined(HANA_OS_LINUX) || defined(HANA_OS_ANDROID)
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

		native_mode = native_mode & ~search_system_folders;

		// Trying to open with appended decorations

		if (native_mode & append_decorations) {
			native_mode = native_mode & ~append_decorations;

			auto actual_path = decorate(sl);
			handle_ = dlopen(actual_path.c_str(), native_mode);
			if (handle_) {
				return true;
			}
			std::error_code prog_loc_err;
			std::filesystem::path loc = hana::program_location_impl(prog_loc_err);
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
		std::filesystem::path loc = hana::program_location_impl(prog_loc_err);
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
			return hana::program_location_impl(ec);
		}

		return link_map->l_name;
	}
}
