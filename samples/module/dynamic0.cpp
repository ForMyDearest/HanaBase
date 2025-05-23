#include "common.hpp"

struct DynamicModule0 : hana::IDynamicModule {
	void on_load(int argc, char8_t** argv) override {
		LOG_INFO(u8"dynamic module 0 loaded");
	}

	void on_unload() override {
		LOG_INFO(u8"dynamic module 0 unloaded");
	}

	int main_module_exec(int argc, char8_t** argv) override {
		LOG_INFO(u8"dynamic module 0 executed as main module");
		return 0;
	}
};

HANA_DYNAMIC_MODULE_METADATA(dynamic0) = u8R"(
{
    "api" : "",
    "version" : "",
    "prettyname" : "dynamic0",
    "linking" : "dynamic0",
    "license" : "MIT",
    "url" : "",
    "copyright" : "",
    "author" : "",
    "dependencies" : [{"name" : "HanaBase",  "kind" : "shared"}]
}
)";

HANA_IMPLEMENT_DYNAMIC_MODULE(dynamic0, DynamicModule0)
