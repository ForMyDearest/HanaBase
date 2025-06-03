#if defined(_WIN32)
#	include "thread/win.cpp"
#elif defined(__APPLE__)
#	include "thread/apple.cpp"
#elif defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#	include "thread/unix.cpp"
#endif
