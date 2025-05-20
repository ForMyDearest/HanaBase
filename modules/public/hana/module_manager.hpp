#pragma once

#include "config.h"

namespace hana
{
	struct HANA_BASE_API ModuleManager {
		static void set_root(const char8_t* path);
		static void enable_hotfix_for_module(const char8_t* name);

		static void load(const char8_t* name, bool shared, int argc = 0, char8_t** argv = nullptr);
		static int execute(const char8_t* name, int argc = 0, char8_t** argv = nullptr);
		static bool update();
		static void unload(const char8_t* name);
		static void destroy();

		[[nodiscard]] static const IModule* get_module(const char8_t* name) noexcept;
		[[nodiscard]] static const class Graph* get_dependency_graph() noexcept;
	};
}

#define HANA_META_PREFIX						__hana_module_meta__
#define HANA_DYNAMIC_MODULE_CREATION_PRIFIX		__hana_initialize_module__
