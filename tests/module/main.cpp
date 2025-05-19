#include <doctest/doctest.h>
#include <hana/log.hpp>

#include "static0.hpp"

#include <filesystem>

struct ModuleTest {
	ModuleTest() {
		hana::LogSystem::set_header_pattern(u8"[{l}] {M}");
	}
};

//       dynamic0
//      /       \
//    |/_       _\|
// static0 <-- dynamic1
//    \         /
//	  _\|     |/_
//     dynamic2
TEST_CASE_FIXTURE(ModuleTest, "dependency") {
	using namespace hana;

	LOG_INFO(u8"\n\n------ dependency ------");
	std::error_code ec = {};
	auto path = std::filesystem::current_path(ec);
	ModuleManager::set_root(path.u8string().c_str());
	ModuleManager::load(u8"dynamic2", true);
	CHECK_EQ(ModuleManager::execute(u8"dynamic2"), 2);
	ModuleManager::unload(u8"dynamic1");
	ModuleManager::destroy();
	LogSystem::poll(true);
}

#include <hana/shared_library.hpp>

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
TEST_CASE_FIXTURE(ModuleTest, "dynamic_patch") {
	using namespace hana;

	LOG_INFO(u8"\n\n------ dynamic_patch ------");
	std::error_code ec = {};
	auto path = std::filesystem::current_path(ec);
	ModuleManager::set_root(path.u8string().c_str());
	ModuleManager::load(u8"dynamic2", true);
	CHECK_EQ(ModuleManager::execute(u8"dynamic1"), 0);
	LOG_INFO(u8"----begins dynamic patch----");
	ModuleManager::enable_hotfix_for_module(u8"hotfix");
	ModuleManager::load(u8"hotfix", true);
	LOG_INFO(u8"----ends dynamic patch----");

	while (true) {
		IHotfixModule* hotfix_module = (IHotfixModule*) ModuleManager::get_module(u8"hotfix");
		auto tikc_func = hotfix_module->shared_lib->get<bool(IHotfixModule*)>(u8"HotfixModuleTick");
		if (tikc_func(hotfix_module)) break;
		ModuleManager::update();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	ModuleManager::unload(u8"hotfix");
	ModuleManager::destroy();
	LogSystem::poll(true);
}
