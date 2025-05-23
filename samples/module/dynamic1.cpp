#include "common.hpp"

struct DynamicModule1 : hana::IDynamicModule {
	void on_load(int argc, char8_t** argv) override {
		LOG_INFO(u8"dynamic module 1 loaded");
	}

	void on_unload() override {
		LOG_INFO(u8"dynamic module 1 unloaded");
	}
};

HANA_DYNAMIC_MODULE_METADATA(dynamic1) = u8R"(
{
    "api" : "",
    "version" : "",
    "prettyname" : "dynamic1",
    "linking" : "dynamic1",
    "license" : "MIT",
    "url" : "",
    "copyright" : "",
    "author" : "",
    "dependencies" : [{"name" : "dynamic0",  "kind" : "shared"}]
}
)";

HANA_IMPLEMENT_DYNAMIC_MODULE(dynamic1, DynamicModule1)
