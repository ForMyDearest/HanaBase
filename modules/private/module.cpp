#include "misc.hpp"
#include "module.hpp"

#include <hana/log.hpp>
#include <hana/hash.hpp>
#include <hana/json.hpp>
#include <hana/module_manager.hpp>

#define CR_HOST
#include <cr.h>

// default module
namespace hana
{
	const char8_t* IDynamicModule::get_meta_data() const {
		const auto meta_symbol = HString::concat(HANA_MAKE_UTF8(HANA_META_PREFIX), name);
		return shared_lib->get<const char8_t*>(meta_symbol.data());
	}

	void DefaultDynamicModule::on_load(int argc, char8_t** argv) {
		LOG_TRACE(u8"[default implementation] Dynamic module ({}) loaded", name);
	}

	int DefaultDynamicModule::main_module_exec(int argc, char8_t** argv) {
		LOG_TRACE(u8"[default implementation] Dynamic module ({}) executed", name);
		return 0;
	}

	void DefaultDynamicModule::on_unload() {
		LOG_TRACE(u8"[default implementation] Dynamic module ({}) unloaded", name);
	}
}

// util
namespace hana
{
	std::filesystem::path get_version_path(const std::filesystem::path& basepath, const unsigned version, const std::filesystem::path& temppath) {
		auto&& basePath = basepath.lexically_normal();
		std::filesystem::path fname = basePath.stem();
		fname += std::to_string(version);
		fname += basePath.extension();

		return temppath.empty() ? basePath.parent_path() / fname : temppath / fname;
	}

	std::filesystem::path get_file_name(const HString& name) {
		std::filesystem::path path;
#if (AUXILIARY_PLATFORM_MACOSX == 1) || (AUXILIARY_PLATFORM_UNIX == 1)
		path += u8"lib";
#endif
		path += name.data();
		path += suffix();
		return path;
	}

	size_t do_hash(const HStringView& str) {
		return Hash<HStringView>()(str);
	}

	bool process_pdb(const std::filesystem::path& dst) {
#ifdef _MSC_VER
		auto basePath = dst.lexically_normal();

		auto folder = basePath.parent_path();
		auto fname = basePath.stem();
		// replace ext with .pdb
		auto pdbDst = folder / (fname.string() + ".pdb");
		std::string orig_pdb;
		bool result = cr_pdb_replace(dst.string(), fname.string() + ".pdb", orig_pdb);
		std::error_code ec;
		std::filesystem::copy(orig_pdb, pdbDst, std::filesystem::copy_options::overwrite_existing, ec);
		if (ec) {
			LOG_ERROR(u8"copy pdb file failed: {}", ec.message());
			result = false;
		}
		return result;
#else
		return false;
#endif
	}
}

// internal function
namespace hana
{
	bool ModuleManagerImpl::LoadHotfixModule(SharedLibrary* lib, size_t hash, std::filesystem::path path) {
		std::error_code ec;
		if (!std::filesystem::exists(path, ec)) {
			LOG_ERROR(u8"Hotfix module ({}) not found!", path.string());
			return false;
		}

		auto&& ctx = hotfix_contexts[hash];
		const std::filesystem::path new_path = get_version_path(path, ctx.version, ctx.temppath);
		if (true) {
			ctx.last_working_version = ctx.version;
			std::filesystem::copy(path, new_path, std::filesystem::copy_options::overwrite_existing, ec);
			if (ec) {
				LOG_ERROR(u8"Hotfix module ({}) rename failed! Reason: {}", path.u8string(), ec.message());
				return false;
			}
			ctx.next_version = ctx.next_version + 1;
#ifndef NDEBUG
			if (!process_pdb(new_path)) {
				LOG_ERROR(u8"Hotfix module ({}) pdb process failed, debugging may be affected and/or reload may fail!", path.u8string());
			}
#endif
		}
		if (!lib->load(new_path.u8string().c_str())) {
			LOG_ERROR(u8"Hotfix module ({}) load failed! Reason: {}", new_path.string(), get_last_error());
			return false;
		}
		ctx.timestamp = std::filesystem::last_write_time(new_path, ec);
		ctx.version = ctx.next_version - 1;
		ctx.path = std::move(path);
		return true;
	}

	IModule* ModuleManagerImpl::LoadModule(HString name, const bool shared, const bool load, const int argc, char8_t** argv) {
		// module existed
		auto hash = do_hash(name);
		if (const auto iter = modules_map.find(hash); iter != modules_map.end()) {
			assert(iter->second->is_shared() == shared);
			return iter->second;
		}

		const bool hotfix = hotfix_contexts.contains(hash);
		IModule* module;

		// LoadSharedModule
		if (shared) {
			auto&& sharedLib = shared_libs[hash];
			HString init_func_name = HString::concat(HANA_MAKE_UTF8(HANA_DYNAMIC_MODULE_CREATION_PRIFIX), name);
			const auto meta_symbol = HString::concat(HANA_MAKE_UTF8(HANA_META_PREFIX), name);
			const bool is_proc_mod = process_symbol_table.has_symbol(meta_symbol.data());

			// maybe nullptr
			auto* func = static_cast<IModule*(*)()>(process_symbol_table.get_symbol(init_func_name.data()));
			if (hotfix && (is_proc_mod || func)) {
				LOG_ERROR(u8"Hotfix module ({}) load failed! Reason: module already loaded!", name);
			}

#ifndef SHIPPING_ONE_ARCHIVE
			if (!is_proc_mod && !func) {
				// try load dll

				auto&& filename = get_file_name(name);
				if (!hotfix) {
					if (!sharedLib.load((root / filename).u8string().data())) {
						LOG_ERROR(u8"Shared library ({}) load failed! Reason: {}", filename.u8string(), get_last_error());
					} else {
						LOG_TRACE(u8"Load dll ({}) success", filename.u8string());
						func = static_cast<IModule*(*)()>(sharedLib.get_symbol(init_func_name.data()));
					}
				} else if (LoadHotfixModule(&sharedLib, hash, std::move(filename))) {
					LOG_TRACE(u8"Hotfix module ({}) load success", name);
					func = static_cast<IModule*(*)()>(sharedLib.get_symbol(init_func_name.data()));
				}
			}
#endif

			if (func) {
				module = func();
			} else {
				LOG_DEBUG(u8"No user defined symbol: {}", init_func_name);
				module = new DefaultDynamicModule;
			}

			if (hotfix) {
				hotfixs.emplace(hash);
			}
			reinterpret_cast<IDynamicModule*>(module)->shared_lib = &sharedLib;
		}
		// LoadStaticModule
		else {
			const auto init_func = static_init_map.find(hash);
			if (init_func == static_init_map.end()) {
				LOG_FATAL(u8"Static module {} has no initial function!", name);
			}
			module = init_func->second();
		}

		module->name = std::move(name);
		ParseMetaData(module, hash, load, argc, argv);
		assert(hotfix <= module->is_reloadable());
		if (load) module->on_load(argc, argv);
		modules_map.emplace(hash, module);

		for (auto&& func: subsystem_create_map[hash]) {
			module->subsystems.emplace_back(func());
		}
		for (auto&& subsystem: module->subsystems) {
			subsystem->init();
		}

		return module;
	}

	void ModuleManagerImpl::ParseMetaData(IModule* module, size_t hash, bool load, int argc, char8_t** argv) {
		JsonReader reader(module->get_meta_data());

		reader.start_object(u8"");
		if (true) {
			json_read(reader, u8"api", module->api);
			json_read(reader, u8"version", module->version);
			json_read(reader, u8"prettyname", module->prettyname);
			json_read(reader, u8"linking", module->linking);
			json_read(reader, u8"license", module->license);
			json_read(reader, u8"url", module->url);
			json_read(reader, u8"copyright", module->copyright);
			json_read(reader, u8"author", module->author);

			size_t count;
			reader.start_array(u8"dependencies", count);
			if (count == 0) roots.emplace(hash);

			for (auto i = 0; i < count; i++) {
				HString name, kind;
				reader.start_object(u8"");
				json_read(reader, u8"name", name);
				json_read(reader, u8"kind", kind);
				reader.end_object();

				auto&& dep_module = LoadModule(std::move(name), kind == u8"shared", load, argc, argv);

				dependency_graph.add_edge(dep_module, module);
			}
			// reader.end_array();
		}
		// reader.end_object();
	}

	void ModuleManagerImpl::UnloadModule(size_t hash) {
		auto&& module_iter = modules_map.find(hash);
		if (module_iter == modules_map.end()) return;

		auto&& module = module_iter->second;
		dependency_graph.foreach_neighbors(module, [this](GraphNode* node) {
			UnloadModule(do_hash(reinterpret_cast<IModule*>(node)->name));
		});

		for (auto&& subsystem: module->subsystems) {
			subsystem->shutdown();
			delete subsystem;
		}
		subsystem_id_map.erase(hash);
		modules_map.erase(module_iter);
		destruction.emplace_back(module, hash);
	}

	void ModuleManagerImpl::DestroyModules() {
		for (auto&& [module, hash]: destruction) {
			if (auto&& iter = hotfixs.find(hash); iter != hotfixs.end()) hotfixs.erase(iter);

			dependency_graph.remove_vertex(module);

			module->on_unload();
			delete module;

			if (auto&& lib_iter = shared_libs.find(hash); lib_iter != shared_libs.end()) {
				lib_iter->second.unload();
				shared_libs.erase(lib_iter);
			}
		}
	}
}

// export function
namespace hana
{
	void ModuleManagerImpl::load(const char8_t* name, const bool shared, const int argc, char8_t** argv) {
		LoadModule(name, shared, true, argc, argv);
	}

	int ModuleManagerImpl::execute(const char8_t* name, const int argc, char8_t** argv) {
		auto&& module_iter = modules_map.find(do_hash(name));
		if (module_iter == modules_map.end()) {
			LOG_ERROR(u8"Module ({}) not exists!", name);
			return -1;
		}
		return module_iter->second->main_module_exec(argc, argv);
	}

	// links: https://fungos.github.io/cr-simple-c-hot-reload/
	// 1.Do not save objects that have pointers to anything that is not in the heap;
	// 2.Do not save objects that have non-trivial constructors and destructors, they may or may not work;
	bool ModuleManagerImpl::update() {
		for (auto hash: hotfixs) {
			auto&& ctx = hotfix_contexts[hash];

			if (std::filesystem::last_write_time(ctx.path) > ctx.timestamp) {
				// unload old module
				const auto this_module = reinterpret_cast<IHotfixModule*>(modules_map[hash]);
				this_module->on_reload_begin();
				for (auto&& subsystem: this_module->subsystems) subsystem->begin_reload();
				const auto this_state = this_module->state;
				// Watch out!
				HString this_name = this_module->name;
				delete this_module;
				auto&& lib = shared_libs[hash];
				lib.unload();

				if (!LoadHotfixModule(&lib, hash, get_file_name(this_name))) {
					return false;
				}

				const HString init_name = HString::concat(HANA_MAKE_UTF8(HANA_DYNAMIC_MODULE_CREATION_PRIFIX), this_name);
				auto* func = static_cast<IModule*(*)()>(lib.get_symbol(init_name.c_str()));

				if (!func) {
					LOG_ERROR(u8"Hotfix module ({}) load failed!", this_name);
					return false;
				}

				const auto new_module = reinterpret_cast<IHotfixModule*>(func());
				new_module->shared_lib = &lib;
				new_module->name = std::move(this_name);
				ParseMetaData(new_module, hash, false);
				modules_map[hash] = new_module;
				for (auto&& create: subsystem_create_map[hash]) {
					new_module->subsystems.emplace_back(create());
				}
				for (auto&& subsystem: new_module->subsystems) {
					subsystem->end_reload();
				}
				new_module->state = this_state;
				new_module->on_reload_finish();

				// reload module
				LOG_TRACE(u8"hotfix {}-v{}", new_module->name, ctx.version);
			}
		}
		return true;
	}

	void ModuleManagerImpl::unload(const char8_t* name) {
		destruction.clear();
		const auto hash = do_hash(name);
		UnloadModule(hash);
		DestroyModules();

		if (auto&& iter = roots.find(hash); iter != roots.end()) {
			roots.erase(iter);
		}
	}

	void ModuleManagerImpl::destroy() {
		destruction.clear();
		for (const auto hash: roots) UnloadModule(hash);
		DestroyModules();
		roots.clear();
	}
}

// setter & getter
namespace hana
{
	ModuleManagerImpl& ModuleManagerImpl::instance() {
		static ModuleManagerImpl instance_;
		return instance_;
	}

	ModuleManagerImpl::ModuleManagerImpl() noexcept {
		if (!process_symbol_table.load(nullptr)) {
			LOG_FATAL(u8"Failed to load symbol table");
		}
	}

	ModuleManagerImpl::~ModuleManagerImpl() noexcept {
		destroy();
	}

	void ModuleManagerImpl::set_root(const char8_t* path) {
		root = path;
	}

	void ModuleManagerImpl::enable_hotfix_for_module(const char8_t* name) {
		hotfix_contexts.emplace(do_hash(name), ModuleContext{});
	}

	void ModuleManagerImpl::register_subsystem(const char8_t* name, const char8_t* id, IModuleSubsystem*(*func)()) {
		const auto hash_name = do_hash(name);

		auto creataion = subsystem_create_map[hash_name];
		for (auto&& pfn: creataion) {
			if (pfn == func) return;
		}

		auto sub_id = subsystem_id_map[hash_name];
		const auto hash_id = do_hash(id);
		for (auto&& ID: sub_id) {
			if (ID == hash_id) return;
		}

		creataion.emplace_back(func);
		sub_id.emplace_back(hash_id);
	}

	void ModuleManagerImpl::register_statically_linked_module(const char8_t* name, IStaticModule*(*func)()) {
		if (const auto hash = do_hash(name); !static_init_map.contains(hash)) {
			static_init_map.emplace(hash, func);
		}
	}

	const IModule* ModuleManagerImpl::get_module(const char8_t* name) const noexcept {
		auto&& iter = modules_map.find(do_hash(name));
		return iter == modules_map.end() ? nullptr : iter->second;
	}

	const Graph* ModuleManagerImpl::get_dependency_graph() const noexcept {
		return &dependency_graph;
	}
}

// export class
namespace hana
{
	void ModuleManager::set_root(const char8_t* path) {
		ModuleManagerImpl::instance().set_root(path);
	}

	void ModuleManager::enable_hotfix_for_module(const char8_t* name) {
		ModuleManagerImpl::instance().enable_hotfix_for_module(name);
	}

	void ModuleManager::load(const char8_t* name, bool shared, int argc, char8_t** argv) {
		ModuleManagerImpl::instance().load(name, shared, argc, argv);
	}

	int ModuleManager::execute(const char8_t* name, int argc, char8_t** argv) {
		return ModuleManagerImpl::instance().execute(name, argc, argv);
	}

	bool ModuleManager::update() {
		return ModuleManagerImpl::instance().update();
	}

	void ModuleManager::unload(const char8_t* name) {
		ModuleManagerImpl::instance().unload(name);
	}

	void ModuleManager::destroy() {
		ModuleManagerImpl::instance().destroy();
	}

	const IModule* ModuleManager::get_module(const char8_t* name) noexcept {
		return ModuleManagerImpl::instance().get_module(name);
	}

	const Graph* ModuleManager::get_dependency_graph() noexcept {
		return ModuleManagerImpl::instance().get_dependency_graph();
	}

	StaticallyLinkedModuleRegistrant::StaticallyLinkedModuleRegistrant(const char8_t* module_name, IStaticModule*(*func)()) {
		ModuleManagerImpl::instance().register_statically_linked_module(module_name, func);
	}

	ModuleSubsystemRegistrant::ModuleSubsystemRegistrant(const char8_t* module_name, const char8_t* subsystem_id, IModuleSubsystem*(*func)()) {
		ModuleManagerImpl::instance().register_subsystem(module_name, subsystem_id, func);
	}
}
