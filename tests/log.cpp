#define HANA_LOG_ENABLE
#include <hana/log.hpp>

void runBenchmark();

#include <iostream>
#include <chrono>

void logcb(int64_t, hana::LogSystem::LogLevel, hana::HStringView, uint32_t, hana::HStringView, hana::HStringView msg) {
	std::cout << "callback full msg: " << msg.raw_data() << std::endl;
}

void logQFullCB(void* userData) {
	std::cout << "log q full" << std::endl;
}

int main() {
	// char randomString[] = "Hello World";
	// LOG_INFO(u8"A string, pointer, number, and float: '{}', {}, {}, {}", randomString, (void*) &randomString, 512, 3.14159);
	//
	// char strarr[10] = "111";
	// char* cstr = strarr;
	// std::string str = "aaa";
	// LOG_INFO(u8"str: {}, pstr: {}, strarr: {}, pstrarr: {}, cstr: {}, pcstr: {}", str, (void*)&str, strarr, (void*)&strarr, cstr, (void*)&cstr);
	// str = "bbb";
	// strcpy(cstr, "222");
	//
	// LOG_DEBUG(u8"This message wont be logged since it is lower than the current log level.");
	// hana::LogSystem::set_log_level(hana::LogSystem::LogLevel::debug);
	// LOG_DEBUG(u8"Now debug msg is shown");
	//
	// hana::LogSystem::poll();
	//
	// hana::LogSystem::set_thread_name(u8"main");
	// LOG_INFO(u8"Thread name changed");
	//
	// hana::LogSystem::poll();
	//
	// hana::LogSystem::set_header_pattern(u8"{m} {l}[{t}] {M}");
	// hana::LogSystem::set_timestamp_precision(hana::LogSystem::TimestampPrecision::ns);
	// LOG_INFO(u8"Header pattern is changed, full date time info is shown");
	//
	// hana::LogSystem::poll();
	//
	// hana::LogSystem::set_log_callback(logcb, hana::LogSystem::LogLevel::warn);
	// LOG_WARN(u8"This msg will be called back");
	//
	// hana::LogSystem::set_log_file(u8"/tmp/wow", true);
	// for (int i = 0; i < 10; i++) {
	// 	LOG_WARN(u8"test logfilepos: {}.", i);
	// }
	//
	// hana::LogSystem::set_log_queue_full_callback(logQFullCB, nullptr);
	// for (int i = 0; i < 1024; i++) {
	// 	std::string str(1000, ' ');
	// 	LOG_INFO(u8"log q full cb test: {}", str);
	// }
	//
	// hana::LogSystem::poll();
	runBenchmark();

	return 0;
}

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
