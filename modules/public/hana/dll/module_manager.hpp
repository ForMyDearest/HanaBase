#pragma once

#include "hana/platform/macros.h"

namespace hana
{
	struct HANA_BASE_API ModuleManager {
		//! @param path If nullptr, automatically set std::filesystem::current_path()
		static void set_root(const char8_t* path = nullptr);
		static void enable_hotfix_for_module(const char8_t* name);

		static void load(const char8_t* name, bool shared, int argc = 0, char8_t** argv = nullptr);
		static int execute(const char8_t* name, int argc = 0, char8_t** argv = nullptr);
		static bool update();
		static void unload(const char8_t* name);
		static void destroy();

		[[nodiscard]] static struct IModule* get_module(const char8_t* name) noexcept;
		[[nodiscard]] static class Graph* get_dependency_graph() noexcept;

		static void dump_graphviz(const char8_t* path);
	};
}
