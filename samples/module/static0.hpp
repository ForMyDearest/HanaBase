#pragma once

#include "common.hpp"


struct StaticModule0 : hana::IStaticModule {
	void on_load(int argc, char8_t** argv) override;
	void on_unload() override;
	const char8_t* get_meta_data() const override;
};

HANA_IMPLEMENT_STATIC_MODULE(static0, StaticModule0)
