#include "platform/crash/common.hpp"

#include <hana/log.hpp>
#include <hana/dll/module.hpp>

inline constexpr const char8_t* kHanaBaseMetaInfo = u8R"(
{
    "api" : "0.0.2",
    "version" : "0.1.0",
    "prettyname" : "base",
    "linking" : "HanaBase",
    "license" : "MIT",
    "url" : "https://github.com/ForMyDearest/HanaBase",
    "copyright" : "(c) 2025 Hanasaka Yui",
    "author" : "MILLS",
    "dependencies" : []
}
)";

#ifdef HANA_BASE_DLL

namespace hana
{
	struct HanaBaseModule : IHotfixModule {
		void on_load(int argc, char8_t** argv) override;
		void on_unload() override;
		void on_reload_begin() override {}
		void on_reload_finish() override {}
	};
}

HANA_IMPLEMENT_DYNAMIC_MODULE(HanaBase, hana::HanaBaseModule)
HANA_DYNAMIC_MODULE_METADATA(HanaBase) = kHanaBaseMetaInfo;

#else

#include <hana/static_modules/base.hpp>

namespace hana
{
	const char8_t* HanaBaseModule::get_meta_data() const {
		return kHanaBaseMetaInfo;
	}
}

#endif

namespace hana
{
	void HanaBaseModule::on_load(int argc, char8_t** argv) {
		init_crash_handle();
		LOG_INFO(u8"hana base module loaded");
	}

	void HanaBaseModule::on_unload() {
		LOG_INFO(u8"hana base module unloaded");
		shutdown_crash_handle();
		LogSystem::poll();
	}
}
