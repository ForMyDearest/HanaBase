#define HANA_LOG_ENABLE
#include <hana/log.hpp>
#include <hana/platform/thread.hpp>

#include <chrono>
#include <iostream>

void runBenchmark() {
	constexpr int RECORDS = 100000;
	hana::LogSystem::close_log_file();
	hana::LogSystem::set_log_callback(nullptr, hana::LogSystem::LogLevel::warn);

	const auto t0 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < RECORDS; ++i) {
		LOG_INFO(u8"Simple log message with one parameters, {}", i);
	}
	const auto t1 = std::chrono::high_resolution_clock::now();

	double span = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0).count();
	hana::LogSystem::poll(true);
	std::cout << "benchmark, front latency: " << (span / RECORDS) * 1e9 << " ns/msg average\n";
}

int main() {
	// using namespace hana;
	//
	// std::vector<Thread*> ts;
	//
	// LogSystem::set_header_pattern(u8"[{t} {T}] {M}");
	//
	// ts.emplace_back(Thread::init({
	// 	.func = [](void*) {
	// 		Thread::set_current_name(u8"thread1");
	//
	// 		LOG_INFO(u8"124");
	// 	}
	// }));
	//
	// ts.emplace_back(Thread::init({
	// 	.func = [](void*) {
	// 		Thread::set_current_name(u8"thread2");
	// 		LOG_INFO(u8"222");
	// 	}
	// }));
	//
	// for(auto& t : ts) {
	// 	t->destroy();
	// }
	//
	// LogSystem::poll();
	runBenchmark();
}
