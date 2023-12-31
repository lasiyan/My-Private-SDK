#ifndef __ROWEN_SDK_CORE_LOGGER_HPP__
#define __ROWEN_SDK_CORE_LOGGER_HPP__

#include <string>
#include <unordered_map>

namespace rs {

class Logger {
 public:
  enum class Level { OFF, FATAL, ERROR, WARN, INFO, DEBUG, TRACE, RAW };
  enum class Target { CONSOLE = 1, FILE, CONSOLE_FILE };

  struct Directory {
    int same_time_files = 0;
    int previous_time   = -1;
  };

 public:
  Logger();

  //////////////////////////
  // Options & Control

  // If want to change directory (default : ./log)
  static void setDirectory(const std::string& logging_directory);
  static void resetDirectory();

  // Set Logger Level
  static void setLevel(Level level);

  // Set Logger Target (File or Console or Both)
  static void setTarget(Target target);

  // Enable tarce logging
  static void setTraceLogging(bool enable);

  // Enable header date information
  static void setHeaderDate(bool enable);

  //////////////////////////

  // clang-format off
  // wrapper
  template <typename... Args> static void fatal(const char* fmt, Args... args)  { log(Level::FATAL, false, nullptr, 0, fmt, args...); }
  template <typename... Args> static void error(const char* fmt, Args... args)  { log(Level::ERROR, false, nullptr, 0, fmt, args...); }
  template <typename... Args> static void warn(const char* fmt, Args... args)   { log(Level::WARN,  false, nullptr, 0, fmt, args...); }
  template <typename... Args> static void info(const char* fmt, Args... args)   { log(Level::INFO,  false, nullptr, 0, fmt, args...); }
  template <typename... Args> static void debug(const char* fmt, Args... args)  { log(Level::DEBUG, false, nullptr, 0, fmt, args...); }
  template <typename... Args> static void trace(const char* fmt, Args... args)  { log(Level::TRACE, false, nullptr, 0, fmt, args...); }

 public:
  // wrapper (no time-line & file location)
  template <typename... Args> static void fatal_raw(const char* fmt, Args... args)  { log(Level::FATAL, true, nullptr, 0, fmt, args...); }
  template <typename... Args> static void error_raw(const char* fmt, Args... args)  { log(Level::ERROR, true, nullptr, 0, fmt, args...); }
  template <typename... Args> static void warn_raw(const char* fmt, Args... args)   { log(Level::WARN,  true, nullptr, 0, fmt, args...); }
  template <typename... Args> static void info_raw(const char* fmt, Args... args)   { log(Level::INFO,  true, nullptr, 0, fmt, args...); }
  template <typename... Args> static void debug_raw(const char* fmt, Args... args)  { log(Level::DEBUG, true, nullptr, 0, fmt, args...); }
  template <typename... Args> static void trace_raw(const char* fmt, Args... args)  { log(Level::TRACE, true, nullptr, 0, fmt, args...); }
  // clang-format on

 public:
  static void log(Level level, bool raw, const char* file, int line,
                  const char* fmt, ...);

  static std::string extractClass(const std::string& prettyFunction);
  static std::string extractMethod(const std::string& prettyFunction);

 private:
  static std::string getSafeDirectory(const std::string& path);
  static void        assertDirectory(const std::string& path);
  static void        removeLambda(std::string& prettyFunction);

 private:
  static Level  option_logging_level_;
  static Target option_logging_target_;
  static bool   option_enable_tarce_logging_;
  static bool   option_enable_header_date_;

  static char                                       directory_[260];
  static std::unordered_map<std::string, Directory> directories_;

  static unsigned int startup_count_;
  static int          previous_hour_;
};

};  // namespace rs

static rs::Logger logger;

#ifdef _WIN32
  #define __PRETTY_FUNCTION__ __func__
#endif

#define __FILENAME__ \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define __CLASS__  (rs::Logger::extractClass(__PRETTY_FUNCTION__).c_str())
#define __METHOD__ (rs::Logger::extractMethod(__PRETTY_FUNCTION__).c_str())

// clang-format off
#define logger_fatal(fmt, ...) rs::Logger::log(rs::Logger::Level::FATAL, false, __FILENAME__, __LINE__, (const char*)fmt, ##__VA_ARGS__)
#define logger_error(fmt, ...) rs::Logger::log(rs::Logger::Level::ERROR, false, __FILENAME__, __LINE__, (const char*)fmt, ##__VA_ARGS__)
#define logger_warn(fmt, ...)  rs::Logger::log(rs::Logger::Level::WARN,  false, __FILENAME__, __LINE__, (const char*)fmt, ##__VA_ARGS__)
#define logger_info(fmt, ...)  rs::Logger::log(rs::Logger::Level::INFO,  false, __FILENAME__, __LINE__, (const char*)fmt, ##__VA_ARGS__)
#define logger_debug(fmt, ...) rs::Logger::log(rs::Logger::Level::DEBUG, false, __FILENAME__, __LINE__, (const char*)fmt, ##__VA_ARGS__)
#define logger_trace(fmt, ...) rs::Logger::log(rs::Logger::Level::TRACE, false, __FILENAME__, __LINE__, (const char*)fmt, ##__VA_ARGS__)

#define logger_errno(_TITLE_, _STR_)	{ logger.error("%s : %s (%d, %s)", _TITLE_, _STR_, errno, strerror(errno)); }
// clang-format on

#endif  //__ROWEN_SDK_CORE_LOGGER_HPP__