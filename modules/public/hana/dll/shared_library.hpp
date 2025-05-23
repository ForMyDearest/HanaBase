#pragma once

#include "hana/platform/macros.h"

#include <filesystem>

#ifdef _WIN32
#	include <windows.h>
#else
#   include <dlfcn.h>
#endif

namespace hana
{
	class HANA_BASE_API SharedLibrary {
	public:
		enum LoadMode {
#ifdef _WIN32
			default_mode                  = 0,
			dont_resolve_dll_references   = DONT_RESOLVE_DLL_REFERENCES,
			load_ignore_code_authz_level  = LOAD_IGNORE_CODE_AUTHZ_LEVEL,
			load_with_altered_search_path = LOAD_WITH_ALTERED_SEARCH_PATH,
			rtld_lazy                     = 0,
			rtld_now                      = 0,
			rtld_global                   = 0,
			rtld_local                    = 0,
			rtld_deepbind                 = 0,
			append_decorations            = 0x00800000,
			search_system_folders         = (append_decorations << 1)
#else
			default_mode                  = 0,
			dont_resolve_dll_references   = 0,
			load_ignore_code_authz_level  = 0,
			load_with_altered_search_path = 0,
			rtld_lazy                     = RTLD_LAZY,
			rtld_now                      = RTLD_NOW,
			rtld_global                   = RTLD_GLOBAL,
			rtld_local                    = RTLD_LOCAL,
			rtld_deepbind                 = RTLD_DEEPBIND,
			append_decorations            = 0x00800000,
			search_system_folders         = (append_decorations << 1)
#endif
		};

#ifdef _WIN32
		using native_handle_t = HMODULE;
#else
		using native_handle_t = void*;
#endif

		constexpr SharedLibrary() noexcept = default;
		constexpr SharedLibrary(SharedLibrary&& rhs) noexcept { swap(rhs); }

		constexpr SharedLibrary& operator=(SharedLibrary&& rhs) noexcept {
			swap(rhs);
			return *this;
		}

		SharedLibrary(const SharedLibrary& rhs) = delete;
		const SharedLibrary& operator=(const SharedLibrary& rhs) = delete;

		~SharedLibrary() noexcept { unload(); }

		bool load(const char8_t* path, LoadMode mode = search_system_folders) noexcept;

		bool unload() noexcept;

		[[nodiscard]] constexpr bool is_loaded() const noexcept {
			return this->handle_ != nullptr;
		}

		bool has_symbol(const char8_t* symbol_name) const noexcept;

		void* get_symbol(const char8_t* symbol_name) const noexcept;

		template<typename T>
		T& get(const char8_t* symbol_name) const noexcept {
			const auto address = get_symbol(symbol_name);
			return *static_cast<T*>(address);
		}

		[[nodiscard]] std::filesystem::path get_location() const noexcept;

		[[nodiscard]] constexpr native_handle_t handle() const noexcept {
			return handle_;
		}

		constexpr void swap(SharedLibrary& rhs) noexcept {
			std::swap(rhs.handle_, handle_);
		}

	private:
		native_handle_t handle_{nullptr};
	};

	constexpr SharedLibrary::LoadMode operator|(SharedLibrary::LoadMode lhs, SharedLibrary::LoadMode rhs) noexcept {
		using T = std::underlying_type_t<SharedLibrary::LoadMode>;
		return static_cast<SharedLibrary::LoadMode>(static_cast<T>(lhs) | static_cast<T>(rhs));
	}
}
