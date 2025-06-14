#pragma once

#include <hana/dll/module.hpp>
#include <hana/graph/graph.hpp>
#include <hana/graph/graphviz.hpp>
#include <hana/dll/shared_library.hpp>

#include <parallel_hashmap/phmap.h>

#include <filesystem>

namespace hana
{
	struct DefaultDynamicModule final : IDynamicModule {
		void on_load(int argc, char8_t** argv) override;
		void on_unload() override;
	};

	struct ModuleContext {
		std::filesystem::path path = {};
		std::filesystem::path temppath = {};
		std::filesystem::file_time_type timestamp = {};
		unsigned int version = 0;
		unsigned int next_version = 1;
		unsigned int last_working_version = 0;
	};

	struct ModuleManagerImpl {
		static ModuleManagerImpl& instance();

		void set_root(const char8_t* path);
		void enable_hotfix_for_module(const char8_t* name);
		void register_subsystem(const char8_t* name, const char8_t* id, IModuleSubsystem*(*func)());
		void register_statically_linked_module(const char8_t* name, IStaticModule*(*func)());

		void load(const char8_t* name, bool shared, int argc, char8_t** argv);
		int execute(const char8_t* name, int argc, char8_t** argv);
		bool update();
		void unload(const char8_t* name);
		void destroy();

		[[nodiscard]] IModule* get_module(const char8_t* name) noexcept;
		[[nodiscard]] Graph* get_dependency_graph() const noexcept;

		struct GraphvizDumper {
			explicit GraphvizDumper();
			void dump(const char8_t* path);
			graphviz::Node* dfs(IModule* module);

			graphviz::Graph* graph;
			std::unique_ptr<graphviz::Registry> registry;
			phmap::flat_hash_map<IModule*, graphviz::Node*> visited;
		};

		ModuleManagerImpl() noexcept;
		~ModuleManagerImpl() noexcept;

		bool LoadHotfixModule(SharedLibrary* lib, size_t hash, std::filesystem::path path);
		IModule* LoadModule(HString name, bool shared, bool load, int argc = 0, char8_t** argv = nullptr);
		void ParseMetaData(IModule* module, size_t hash, bool load, int argc = 0, char8_t** argv = nullptr);
		void UnloadModule(size_t hash);
		void DestroyModules();

		RCUnique<Graph> dependency_graph;
		SharedLibrary process_symbol_table;
		std::filesystem::path root; // 动态库根目录

		std::vector<std::pair<IModule*, size_t>> destruction;

		phmap::flat_hash_set<size_t> roots; // 依赖图顶层
		phmap::flat_hash_set<size_t> hotfixs;
		phmap::flat_hash_map<size_t, IModule*> modules_map;
		phmap::flat_hash_map<size_t, ModuleContext> hotfix_contexts;

		phmap::flat_hash_map<size_t, std::vector<size_t>> subsystem_id_map;
		phmap::flat_hash_map<size_t, IStaticModule* (*)()> static_init_map;
		phmap::flat_hash_map<size_t, std::vector<IModuleSubsystem* (*)()>> subsystem_create_map;
	};
}
