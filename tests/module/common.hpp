#pragma once

#include <hana/module.hpp>
#include <hana/format.hpp>
#include <hana/module_manager.hpp>

#include <iostream>

namespace Module
{
	template<typename... Args>
	void println(hana::fmt::format_string<Args...> fmt, Args&&... args) {
		std::cout << hana::format(fmt, std::forward<Args>(args)...).raw_data() << std::endl;
	}
}

#define LOG_TRACE(format, ...)	Module::println(format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...)	Module::println(format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)	Module::println(format, ##__VA_ARGS__)
#define LOG_WARN(format, ...)	Module::println(format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...)	Module::println(format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...)	Module::println(format, ##__VA_ARGS__)
