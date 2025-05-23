#include <hana/log.hpp>
#include <hana/dll/shared_library.hpp>

#include "static0.hpp"

#include <thread>

//       dynamic0
//      /       \
//    |/_       _\|
// static0 <-- dynamic1
//    \         /
//	  _\|     |/_
//     dynamic2
void dependency() {
	using namespace hana;

	LOG_INFO(u8"\n\n------ dependency ------");
	ModuleManager::set_root();
	ModuleManager::load(u8"dynamic2", true);
	LOG_INFO(u8"{}", ModuleManager::get_module(u8"HanaBase")->url);
	ModuleManager::execute(u8"dynamic2");
	ModuleManager::unload(u8"dynamic1");
	ModuleManager::destroy();
}

//       dynamic0
//      /       \
//    |/_       _\|
// static0 <-- dynamic1
//    \         /
//	  _\|     |/_
//     dynamic2   dynamic3
//        \         /
//	      _\|     |/_
//          hotfix
void dynamic_patch() {
	using namespace hana;

	LOG_INFO(u8"\n\n------ dynamic_patch ------");
	ModuleManager::set_root();
	ModuleManager::load(u8"dynamic2", true);
	ModuleManager::execute(u8"dynamic1");
	LOG_INFO(u8"----begins dynamic patch----");
	ModuleManager::enable_hotfix_for_module(u8"hotfix");
	ModuleManager::load(u8"hotfix", true);
	ModuleManager::dump_graphviz(u8"modules.dot");
	LOG_INFO(u8"----ends dynamic patch----");

	while (true) {
		IHotfixModule* hotfix_module = (IHotfixModule*) ModuleManager::get_module(u8"hotfix");
		auto tikc_func = hotfix_module->shared_library.get<bool(IHotfixModule*)>(u8"HotfixModuleTick");
		if (tikc_func(hotfix_module)) break;
		ModuleManager::update();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	ModuleManager::unload(u8"hotfix");
	ModuleManager::destroy();
}

int main() {
	hana::LogSystem::set_header_pattern(u8"[{l}] {M}");
	dependency();
	dynamic_patch();
}
