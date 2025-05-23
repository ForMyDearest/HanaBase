#include "platform/misc.hpp"

#include <hana/dll/shared_library.hpp>

#ifdef _WIN32
#	include "shared_library/win.cpp"
#else
#	include "shared_library/posix.cpp"
#endif
