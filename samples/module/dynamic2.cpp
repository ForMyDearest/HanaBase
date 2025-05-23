#include "common.hpp"

struct DynamicModule2 : hana::IDynamicModule {
	void on_load(int argc, char8_t** argv) override {
		LOG_INFO(u8"dynamic module 2 loaded");
	}

	void on_unload() override {
		LOG_INFO(u8"dynamic module 2 unloaded");
	}

	int main_module_exec(int argc, char8_t** argv) override {
		LOG_INFO(u8"dynamic module 2 executed as main module");
		return 0;
	}
};

HANA_DYNAMIC_MODULE_METADATA(dynamic2) = u8R"(
{
    "api" : "",
    "version" : "",
    "prettyname" : "dynamic2",
    "linking" : "dynamic2",
    "license" : "MIT",
    "url" : "",
    "copyright" : "",
    "author" : "",
    "dependencies" : [
        {"name" : "static0",  "kind" : "static"},
        {"name" : "dynamic1", "kind" : "shared"}
    ]
}
)";

HANA_IMPLEMENT_DYNAMIC_MODULE(dynamic2, DynamicModule2)
