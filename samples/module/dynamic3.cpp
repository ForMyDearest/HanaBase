#include "common.hpp"

struct DynamicModule3 : hana::IDynamicModule {
	void on_load(int argc, char8_t** argv) override {
		LOG_INFO(u8"dynamic module 3 loaded");
	}

	void on_unload() override {
		LOG_INFO(u8"dynamic module 3 unloaded");
	}
};

HANA_DYNAMIC_MODULE_METADATA(dynamic3) = u8R"(
{
    "api" : "",
    "version" : "",
    "prettyname" : "dynamic3",
    "linking" : "dynamic3",
    "license" : "MIT",
    "url" : "",
    "copyright" : "",
    "author" : "",
    "dependencies" : [{"name" : "HanaBase",  "kind" : "shared"}]
}
)";
