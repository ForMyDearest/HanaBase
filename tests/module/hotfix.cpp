#include "common.hpp"

struct State {
	int64_t counter;
};

struct HotfixModule : hana::IHotfixModule {
	State* get_state() const;
	bool tick();

	void on_load(int argc, char8_t** argv) override;
	void on_unload() override;
	void on_reload_begin() override;
	void on_reload_finish() override;
};

State* HotfixModule::get_state() const {
	return (State*) state;
}

bool HotfixModule::tick() {
	auto& counter = get_state()->counter;
	counter += 3;
	if (counter > 20) {
		counter = 0;
		LOG_INFO(u8"hotfix module tick: reset");
		return true;
	}
	LOG_INFO(u8"hotfix module tick: {}", get_state()->counter);
	return false;
}

void HotfixModule::on_load(int argc, char8_t** argv) {
	LOG_INFO(u8"hotfix module loaded");
	state = new State;
	get_state()->counter = 0;
}

void HotfixModule::on_unload() {
	LOG_INFO(u8"hotfix module unloaded");
	delete get_state();
}

void HotfixModule::on_reload_begin() {
	LOG_INFO(u8"hotfix module reload begin");
}

void HotfixModule::on_reload_finish() {
	LOG_INFO(u8"hotfix module reload finish");
}

HANA_IMPLEMENT_DYNAMIC_MODULE(hotfix, HotfixModule)

HANA_DYNAMIC_MODULE_METADATA(hotfix) = u8R"(
{
    "api" : "",
    "version" : "",
    "prettyname" : "hotfix",
    "linking" : "hotfix",
    "license" : "MIT",
    "url" : "",
    "copyright" : "",
    "author" : "",
    "dependencies" : [
        {"name" : "dynamic2",  "kind" : "shared"},
        {"name" : "dynamic3",  "kind" : "shared"}
    ]
}
)";

extern "C" __declspec(dllexport) bool HotfixModuleTick(hana::IHotfixModule* module) {
	return reinterpret_cast<HotfixModule*>(module)->tick();
}
