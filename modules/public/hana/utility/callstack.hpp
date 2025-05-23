#pragma once

#include "hana/platform/macros.h"
#include "hana/container/string_view.hpp"

namespace hana
{
	HANA_BASE_API HStringView get_callstack(unsigned begin = 0, unsigned max_depth = 32);
}
