#define HANA_LOG_ENABLE
#include <hana/log.hpp>
#include <hana/platform/process.hpp>

int main(int argc, char8_t** argv) {
	using namespace hana;
	LogSystem::set_header_pattern(u8"[{l}] {M}");

	LOG_INFO(u8"{} {}", get_current_process_id(), get_current_process_name())

	if (argc == 1) {
		const char8_t* provider_arguments[] = {argv[0], u8"provider", u8"-1"};

		auto provider = start_process(provider_arguments, u8"log/provider.log");
		const auto provider_id = get_process_id(provider);
		auto provider_name = format(u8"{}", provider_id);

		const char8_t* receiver_arguments[] = {argv[0], u8"receiver", provider_name.data()};
		auto receiver = start_process(receiver_arguments, u8"log/receiver.log");

		LOG_INFO(u8"{} {}", provider_id, get_process_id(receiver));

		auto provider_result = wait_process(provider);
		auto receriver_result = wait_process(receiver);

		LOG_INFO(u8"{}", receriver_result + provider_result);
	} else {
		LOG_INFO(u8"arguments : {} {}", argv[1], argv[2]);
		return 1;
	}
}
