#include "static0.hpp"


void StaticModule0::on_load(int argc, char8_t** argv) {
	LOG_INFO(u8"static module 0 loaded");
}

void StaticModule0::on_unload() {
	LOG_INFO(u8"static module 0 unloaded");
}

const char8_t* StaticModule0::get_meta_data() const {
	return u8R"(
{
    "api" : "",
    "version" : "",
    "prettyname" : "static0",
    "linking" : "static0",
    "license" : "MIT",
    "url" : "",
    "copyright" : "",
    "author" : "",
    "dependencies" : [
        {"name" : "dynamic1", "kind" : "shared"},
        {"name" : "dynamic0", "kind" : "shared"}
    ]
}
)";
}
