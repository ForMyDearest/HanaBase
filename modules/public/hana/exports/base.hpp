#pragma once

#include <hana/dll/module.hpp>

namespace hana
{
	class HanaBaseModule : public IStaticModule {
		void on_load(int argc, char8_t** argv) override;
		void on_unload() override;
		const char8_t* get_meta_data() const override;
	};
}

HANA_IMPLEMENT_STATIC_MODULE(HanaBase, hana::HanaBaseModule)
