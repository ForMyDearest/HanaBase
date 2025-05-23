#pragma once

#ifdef _WIN32
#	define HANA_IMPORTS __declspec(dllimport)
#	define HANA_EXPORTS __declspec(dllexport)
#else
#	define HANA_IMPORTS __attribute__((visibility("default")))
#	define HANA_EXPORTS __attribute__((visibility("default")))
#endif

#ifdef _MSC_VER
#   define HANA_FORCEINLINE				__forceinline
#   define HANA_NOINLINE				__declspec(noinline)
#	define HANA_EBCO					__declspec(empty_bases)
#	define HANA_NOVTABLE				__declspec(novtable)
#	ifndef __MINGW32__
#		if (_MSC_VER >= 1900) && !defined(__EDG__)
#			define HANA_ALLOCATOR		__declspec(allocator)
#		else
#			define HANA_ALLOCATOR
#		endif
#		define HANA_RESTRICT			__declspec(restrict)
#	else
#		define HANA_RESTRICT
#	endif
#else
#   define HANA_FORCEINLINE				__inline__ __attribute__((always_inline))
#   define HANA_NOINLINE				__attribute__((noinline))
#	define HANA_EBCO
#	define HANA_NOVTABLE
#	define HANA_ALLOCATOR
#	define HANA_RESTRICT
#endif

#define HANA_STRINGIZING(...)			#__VA_ARGS__
#define HANA_MAKE_STRING(...)			HANA_STRINGIZING(__VA_ARGS__)
#define HANA_FILE_LINE					__FILE__ ":" HANA_MAKE_STRING(__LINE__)
#define HANA_ACTUALLY_JOIN(x, y)		x##y
#define HANA_JOIN(x, y)					HANA_ACTUALLY_JOIN(x, y)

#define HANA_MAKE_UTF8(x)				HANA_JOIN(u8, HANA_MAKE_STRING(x))
#define HANA_MAKE_UTF16(x)				HANA_JOIN(u, HANA_MAKE_STRING(x))
#define HANA_MAKE_UTF32(x)				HANA_JOIN(U, HANA_MAKE_STRING(x))
#define HANA_MAKE_WCHAR(x)				HANA_JOIN(L, HANA_MAKE_STRING(x))

#ifdef __COUNTER__
#   define HANA_UNIQUE_VARNAME(x)		HANA_JOIN(x, __COUNTER__)
#else
#   define HANA_UNIQUE_VARNAME(x)		HANA_JOIN(x, __LINE__)
#endif
