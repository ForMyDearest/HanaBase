#define HANA_LOG_ENABLE
#include <hana/log.hpp>
#include <hana/platform/process.hpp>

int main(int argc, char8_t** argv) {
	using namespace hana;
	LogSystem::set_header_pattern(u8"[{l}] {M}");

	LOG_INFO(u8"{} {}", Process::get_current_pid(), Process::get_current_name());

	if (argc == 1) {
		const char8_t* provider_arguments[] = {argv[0], u8"provider", u8"-1"};

		auto provider = Process::start(provider_arguments, u8"log/provider.log");
		const auto provider_id = provider->get_pid();
		auto provider_name = format(u8"{}", provider_id);

		const char8_t* receiver_arguments[] = {argv[0], u8"receiver", provider_name.data()};
		auto receiver = Process::start(receiver_arguments, u8"log/receiver.log");

		LOG_INFO(u8"{} {}", provider_id, receiver->get_pid());

		auto provider_result = provider->wait();
		auto receriver_result = receiver->wait();

		LOG_INFO(u8"{}", receriver_result + provider_result);
	} else {
		LOG_INFO(u8"arguments : {} {}", argv[1], argv[2]);
		return 1;
	}
}
