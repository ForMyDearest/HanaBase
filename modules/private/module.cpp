#include "platform/crash/common.hpp"

#include <hana/log.hpp>
#include <hana/dll/module.hpp>

namespace hana
{
	struct HanaBaseModule : IDynamicModule {
		void on_load(int argc, char8_t** argv) override {
			init_crash_handle();
			LOG_INFO(u8"hana base module loaded");
		}

		void on_unload() override {
			LOG_INFO(u8"hana base module unloaded");
			shutdown_crash_handle();
		}
	};
}

HANA_IMPLEMENT_DYNAMIC_MODULE(HanaBase, hana::HanaBaseModule)

HANA_DYNAMIC_MODULE_METADATA(HanaBase) = u8R"(
{
    "api" : "0.0.1",
    "version" : "1.0.0",
    "prettyname" : "base",
    "linking" : "HanaBase",
    "license" : "MIT",
    "url" : "https://github.com/ForMyDearest/HanaBase",
    "copyright" : "(c) 2025 Hanasaka Yui",
    "author" : "MILLS",
    "dependencies" : []
}
)";
