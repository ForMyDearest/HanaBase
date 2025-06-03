#pragma once

#include "hana/archive/format.hpp"

namespace hana
{
	struct HANA_BASE_API LogSystem {
		static constexpr char8_t kPatternParam[] = {
			'l', // log level
			'L', // file location
			't', // thread id
			'T', // thread name
			'f', // function name
			'm', // timestamp
			'M', // message
		};

		enum LogLevel : uint8_t {
			trace = 0,
			debug,
			info,
			warn,
			error,
			fatal,
			off
		};

		enum TimestampPrecision : uint8_t {
			none,
			ms,
			us,
			ns
		};

		/*!
		 * @brief
		 *		Preallocate thread queue for current thread
		 * @note
		 *		Since it takes some time to allocate the queue, calling preallocate()
		 *		after creating the thread can reduce the latency of the first log
		 */
		static void preallocate();

		// Set the file for logging
		static void set_log_file(const char8_t* filename, bool truncate = false);

		/*!
		 * @brief
		 *		Set an existing FILE* for logging
		 *
		 * @param manageFp
		 *		if manageFp is false logger will not buffer log
		 *		internally and will not close the FILE*
		 */
		static void set_log_file(FILE* fp, bool manageFp = false);

		// Close the log file and subsequent msgs will not be written into the file,
		// but callback function can still be used
		static void close_log_file();

		// Set current log level, lower level log msgs will be discarded
		static void set_log_level(LogLevel logLevel);

		//! @return Current log level
		static LogLevel get_log_level();

		//! @return True if passed log level is not lower than current log level
		static bool check_log_level(LogLevel level);

		// Set flush delay in nanosecond
		// If there's msg older than ns in the buffer, flush will be triggered
		static void set_flush_delay(int64_t ns);

		// If current msg has level >= flush_log_level, flush will be triggered
		static void set_flush_log_level(LogLevel flush_log_level);

		// If file buffer has more than specified bytes, flush will be triggered
		static void set_flush_buffer_size(uint32_t bytes);

		// callback signature user can register
		typedef void (*LogCBFn)(
			int64_t ns,             // nanosecond timestamp
			LogLevel level,         // logLevel
			HStringView location,   // full file path with line num
			uint32_t tid,           // thread id
			HStringView threadName, // thread name
			HStringView msg         // full log msg with header
		);

		/*!
		 * @brief
		 *		Set a callback function for all log msgs with a mininum log level
		 * @note
		 *		There is a risk of thread race.
		 *		Please add mutex if nessasery.
		 */
		static void set_log_callback(LogCBFn cb, LogLevel minCBLogLevel);

		typedef void (*LogQFullCBFn)(void* userData);
		static void set_log_queue_full_callback(LogQFullCBFn cb, void* userData);

		// Set log header pattern with fmt named arguments
		// TODO : fix bug ( not support character '{' )
		static void set_header_pattern(HStringView pattern);

		static void set_timestamp_precision(TimestampPrecision precision);

		static void vlog(const char8_t* location, const char8_t* function, LogLevel level, HStringView fmt, fmt::format_args args);

		template<typename... Args>
		static void log(const char8_t* location, const char8_t* function, LogLevel level, fmt::format_string<Args...> fmt, Args&&... args) {
			constexpr auto DESC = fmt::make_descriptor<Args...>();
			LogSystem::vlog(location, function, level, fmt.get(), fmt::format_args(fmt::make_format_store(args...), DESC));
		}

		/*!
		 * @brief
		 *		Collect log msgs from all threads and write to log file
		 *
		 * @param force_flush
		 *		If true, internal file buffer is flushed
		 *
		 * @note
		 *		User need to call poll() repeatedly if start_polling_thread is not used
		 */
		static void poll(bool force_flush = false);

		/*!
		 * @brief
		 *		Run a polling thread in the background with a polling interval in ns
		 *
		 * @note
		 *		poll() can't be called after start_polling_thread() is called
		 */
		static void start_polling_thread(int64_t poll_interval = 1000000000);

		// Stop the polling thread
		static void stop_polling_thread();
	};
}

#ifdef HANA_LOG_ENABLE

#ifdef HANA_LOG_CONSOLE

#define HANA_LOG(level, format, color, ...)												\
	do {																				\
		if (!hana::LogSystem::check_log_level(hana::LogSystem::LogLevel::level)) break;	\
		hana::LogSystem::log(															\
			reinterpret_cast<const char8_t*>(HANA_FILE_LINE),							\
			reinterpret_cast<const char8_t*>(__FUNCTION__),								\
			hana::LogSystem::LogLevel::level,											\
			color format u8"\033[0m",													\
			##__VA_ARGS__);																\
	} while (0);

#define LOG_TRACE(format, ...)	HANA_LOG(trace, format, u8"\033[37m", ##__VA_ARGS__)
#define LOG_DEBUG(format, ...)	HANA_LOG(debug, format, u8"\033[32m", ##__VA_ARGS__)
#define LOG_INFO(format, ...)	HANA_LOG(info,  format, u8"\033[34m", ##__VA_ARGS__)
#define LOG_WARN(format, ...)	HANA_LOG(warn,  format, u8"\033[33m", ##__VA_ARGS__)
#define LOG_ERROR(format, ...)	HANA_LOG(error, format, u8"\033[31m", ##__VA_ARGS__)
#define LOG_FATAL(format, ...)	HANA_LOG(fatal, format, u8"\033[30m\033[41m", ##__VA_ARGS__)

#else

#define HANA_LOG(level, format, ...)													\
	do {																				\
		if (!hana::LogSystem::check_log_level(hana::LogSystem::LogLevel::level)) break;	\
		hana::LogSystem::log(															\
			reinterpret_cast<const char8_t*>(HANA_FILE_LINE),							\
			reinterpret_cast<const char8_t*>(__FUNCTION__),								\
			hana::LogSystem::LogLevel::level,											\
			format,																		\
			##__VA_ARGS__																\
		);																				\
	} while (0);

#define LOG_TRACE(format, ...)	HANA_LOG(trace, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...)	HANA_LOG(debug, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)	HANA_LOG(info,  format, ##__VA_ARGS__)
#define LOG_WARN(format, ...)	HANA_LOG(warn,  format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...)	HANA_LOG(error, format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...)	HANA_LOG(fatal, format, ##__VA_ARGS__)

#endif

#endif