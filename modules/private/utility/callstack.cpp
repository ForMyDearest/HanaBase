#include <hana/utility/callstack.hpp>

#include <sstream>

#ifdef _WIN32
#	include <windows.h>
#	include <dbghelp.h>
#else
#	include <vector>
#	include <cxxabi.h>
#	include <execinfo.h>
#endif

namespace hana
{
	HStringView get_callstack(unsigned begin, unsigned max_depth) {
		static std::stringstream callstack;
		callstack.clear();
		callstack.str("");

#ifdef _WIN32
		HANDLE process = GetCurrentProcess();
		HANDLE thread = GetCurrentThread();

		SymInitialize(process, nullptr, TRUE);
		SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);

		CONTEXT context;
		RtlCaptureContext(&context);

		STACKFRAME64 frame = {};
		frame.AddrPC.Offset = context.Rip;
		frame.AddrPC.Mode = AddrModeFlat;
		frame.AddrFrame.Offset = context.Rbp;
		frame.AddrFrame.Mode = AddrModeFlat;
		frame.AddrStack.Offset = context.Rsp;
		frame.AddrStack.Mode = AddrModeFlat;

		for (int i = 0; i < begin; i++) {
			if (!StackWalk64(IMAGE_FILE_MACHINE_AMD64, process, thread, &frame, &context, nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr)) {
				SymCleanup(process);
				return {};
			}
		}

		for (ULONG frame_number = 0; frame_number < max_depth; frame_number++) {
			if (!StackWalk64(IMAGE_FILE_MACHINE_AMD64, process, thread, &frame, &context, nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr))
				break;

			char symbol_buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
			auto symbol = (PSYMBOL_INFO) symbol_buffer;
			symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
			symbol->MaxNameLen = MAX_SYM_NAME;

			DWORD64 displacement = 0;
			IMAGEHLP_MODULE64 module_info;
			module_info.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);

			callstack << '\t' << frame_number << ": ";

			if (SymFromAddr(process, frame.AddrPC.Offset, &displacement, symbol)) {
				if (SymGetModuleInfo64(process, frame.AddrPC.Offset, &module_info))
					callstack << module_info.ModuleName << "!";

				callstack << symbol->Name;

				IMAGEHLP_LINE64 line;
				line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
				DWORD displacement_line = 0;
				if (SymGetLineFromAddr64(process, frame.AddrPC.Offset, &displacement_line, &line)) {
					callstack << " [" << line.FileName << ":" << line.LineNumber << "]";
				}
			} else {
				callstack << "0x" << std::hex << frame.AddrPC.Offset << std::dec;
			}
			callstack << '\n';
		}

		SymCleanup(process);
#else
        // Unix-like systems implementation (unchanged)
		std::vector<void*> array(max_depth)
        size_t size = backtrace(array.data(), max_depth);
        char** strings = backtrace_symbols(array.data(), size);
		strings = strings + begin;

        for (size_t i = 0; i < size; i++)
        {
            callstack << i << ": " << strings[i] << std::endl;
        }

        free(strings);
#endif

		auto sv = callstack.view();
		return {sv.data(), sv.size()};
	}
}
