#pragma once

namespace hana
{
	enum class CrashTerminateCode {
		// POSIX Signals
		Abort          = 1, // abort()
		Interrupt      = 2, // Ctrl-C
		Kill           = 3, // kill
		DividedByZero  = 4, // divide by zero
		IllInstruction = 5, // illegal instruction
		SegFault       = 6, // segmentation fault
		// System Exceptions
		StackOverflow = 7,  // Windows
		Terminate     = 8,  // (not catch) Windows/C++
		Unhandled     = 9,  // (unhandled exception) Windows
		PureVirtual   = 10, // (pure-virtual call) Windows
		OpNewError    = 11, // (new fatal) Windows
		InvalidParam  = 12, // (invalid parameter) Windows
	};
}
