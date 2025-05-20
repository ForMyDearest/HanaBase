#pragma once

#include "graph.hpp"
#include "string.hpp"

namespace hana
{
	struct HANA_NOVTABLE IModuleSubsystem {
		virtual ~IModuleSubsystem() noexcept = default;
		virtual void init() = 0;
		virtual void shutdown() = 0;
		virtual void begin_reload() {}
		virtual void end_reload() {}
	};

	struct HANA_NOVTABLE IModule : GraphNode {
		IModule() = default;
		IModule(const IModule&) = delete;
		IModule& operator=(const IModule&) = delete;
		~IModule() noexcept override = default;

		virtual void on_load(int argc, char8_t** argv) = 0;
		virtual void on_unload() = 0;
		virtual int main_module_exec(int argc, char8_t** argv) { return 0; }
		[[nodiscard]] virtual bool is_shared() const noexcept = 0;
		[[nodiscard]] virtual bool is_reloadable() const noexcept = 0;
		[[nodiscard]] virtual const char8_t* get_meta_data() const = 0;

		HString api;        // version of the engine
		HString version;    // version of the plugin
		HString prettyname; // formatted name of the plugin
		HString linking;    // linking of the plugin
		HString license;    // license of the plugin
		HString url;        // url of the plugin
		HString copyright;  // copyright of the plugin
		HString author;     // copyright of the plugin

		HString name;
		std::vector<IModuleSubsystem*> subsystems;
	};

	struct HANA_NOVTABLE IStaticModule : IModule {
		~IStaticModule() noexcept override = default;
		[[nodiscard]] bool is_shared() const noexcept final { return false; }
		[[nodiscard]] bool is_reloadable() const noexcept final { return false; }
	};

	struct HANA_BASE_API HANA_NOVTABLE IDynamicModule : IModule {
		[[nodiscard]] bool is_shared() const noexcept final { return true; }
		[[nodiscard]] bool is_reloadable() const noexcept override { return false; }
		[[nodiscard]] const char8_t* get_meta_data() const override;

		class SharedLibrary* shared_lib = nullptr;
	};

	struct HANA_NOVTABLE IHotfixModule : IDynamicModule {
		~IHotfixModule() noexcept override = default;
		virtual void on_reload_begin() = 0;
		virtual void on_reload_finish() = 0;

		[[nodiscard]] bool is_reloadable() const noexcept final { return true; }

		void* state = nullptr;
	};

	struct HANA_BASE_API StaticallyLinkedModuleRegistrant {
		StaticallyLinkedModuleRegistrant(const char8_t* module_name, IStaticModule*(*func)());
	};

	struct HANA_BASE_API ModuleSubsystemRegistrant {
		ModuleSubsystemRegistrant(const char8_t* module_name, const char8_t* subsystem_id, IModuleSubsystem*(*func)());
	};
}


#define HANA_IMPLEMENT_STATIC_MODULE(ModuleName, ModuleImplClass)							\
	inline static const hana::StaticallyLinkedModuleRegistrant HanaRegistrant##ModuleName(	\
		HANA_MAKE_UTF8(ModuleName),															\
		[] { return reinterpret_cast<hana::IStaticModule*>(new ModuleImplClass); }			\
	);

#define HANA_DYNAMIC_MODULE_METADATA(ModuleName) \
	extern "C" HANA_EXPORTS const char8_t* HANA_JOIN(HANA_META_PREFIX, ModuleName)

#define HANA_IMPLEMENT_DYNAMIC_MODULE(ModuleName, ModuleImplClass)											\
	extern "C" HANA_EXPORTS hana::IModule* HANA_JOIN(HANA_DYNAMIC_MODULE_CREATION_PRIFIX, ModuleName)() {	\
		return new ModuleImplClass;																			\
	}

#define HANA_IMPLEMENT_MODULE_SUBSYSTEM(ModuleName, Subsystem)						\
	static hana::ModuleSubsystemRegistrant HanaRegistrant##ModuleName##Subsystem(	\
		HANA_MAKE_UTF8(ModuleName),													\
		HANA_MAKE_UTF8(HANA_UNIQUE_VARNAME(Subsystem)),								\
		[] { return reinterpret_cast<hana::IModuleSubsystem*>(new Subsystem); }		\
	);
