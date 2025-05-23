#include <hana/platform/process.hpp>

#if defined(_WIN32)
#   include "process/win.cpp"
#elif defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#   include "process/unix.cpp"
#endif
