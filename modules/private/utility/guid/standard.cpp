#include <memory>
#include <random>

namespace hana
{
	template<typename UniformRandomNumberGenerator>
	class basic_guid_random_generator {
	public:
		using engine_type = UniformRandomNumberGenerator;

		explicit basic_guid_random_generator(engine_type& gen) : generator(&gen, [](auto) {}) {}
		explicit basic_guid_random_generator(engine_type* gen) : generator(gen, [](auto) {}) {}

		[[nodiscard]] guid_t operator()() {
			alignas(uint32_t) uint8_t bytes[16];
			for (int i = 0; i < 16; i += 4) {
				*reinterpret_cast<uint32_t*>(bytes + i) = distribution(*generator);
			}

			// variant must be 10xxxxxx
			bytes[8] &= 0xBF;
			bytes[8] |= 0x80;

			// version must be 0100xxxx
			bytes[6] &= 0x4F;
			bytes[6] |= 0x40;

			return guid_t{std::begin(bytes), std::end(bytes)};
		}

	private:
		std::uniform_int_distribution<uint32_t> distribution;
		std::shared_ptr<UniformRandomNumberGenerator> generator;
	};

	using guid_random_generator = basic_guid_random_generator<std::mt19937>;

	std::optional<guid_t> guid_t::create() noexcept {
		static auto gen = [] {
			std::random_device rd;
			auto seed_data = std::array<int, std::mt19937::state_size>{};
			std::ranges::generate(seed_data, std::ref(rd));
			std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
			std::mt19937 generator(seq);
			return guid_random_generator{generator};
		}();

		return gen();
	}
}
