#include "logger.hpp"

#include <dirent.h>
#include <sys/stat.h>
#include <sys/timeb.h>

#include <cstdarg>
#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>

namespace rs {
const char*    LOGGER_FOLDER_TITEL = "log";
const uint32_t LOGGER_BUFFER_SIZE  = 16384;

std::mutex g_mutex_loggerLock;

// static variables
Logger::Level  Logger::logging_level_  = Logger::Level::INFO;
Logger::Target Logger::logging_target_ = Logger::Target::CONSOLE_FILE;
bool           Logger::logging_trace_  = true;

unsigned int Logger::startup_count_ = 0;
int          Logger::previous_hour_ = -1;
bool         Logger::is_first_file_ = true;

Logger::Logger() { mkdir(LOGGER_FOLDER_TITEL, 0777); }

void Logger::setup(Level level, bool trace, Target target)
{
  logging_level_  = level;
  logging_target_ = target;
  logging_trace_  = trace;
}

////////////////////////////////////////////////////////////////////////////////
// implement
void Logger::log(Level level, bool raw, const char* file, int line,
                 const char* format, ...)
{
  std::unique_lock<std::mutex> ulock(g_mutex_loggerLock);

  try {
    const auto isConsole = [](const Target target) {
      return static_cast<int>(target) & static_cast<int>(Target::CONSOLE);
    };
    const auto isFile = [](const Target target) {
      return static_cast<int>(target) & static_cast<int>(Target::FILE);
    };

    bool write_console = false, write_file = false;

    if (level == Level::FATAL || level == Level::ERROR ||
        level == Level::WARN || level == Level::INFO) {
      if (level <= logging_level_) {
        if (isConsole(logging_target_))
          write_console = true;
        if (isFile(logging_target_))
          write_file = true;
      }
    }
    else if (level == Level::DEBUG) {
      if (level <= logging_level_) {
        if (isConsole(logging_target_))
          write_console = true;
        if (isFile(logging_target_))
          write_file = true;
      }
    }
    else if (level == Level::TRACE && logging_trace_) {
      if (isFile(logging_target_))
        write_file = true;
    }

    if (write_console == false && write_file == false)
      return;

    ///////////
    char content[LOGGER_BUFFER_SIZE];

    std::unique_ptr<char[]> content_extend;
    int                     extension_size = 0;

    // make body
    va_list args, args_temp;
    va_start(args, format);
    va_copy(args_temp, args);
    auto content_size = vsnprintf(NULL, 0, format, args_temp) + 512;
    va_end(args_temp);

    if (content_size > LOGGER_BUFFER_SIZE) {
      extension_size = content_size;
      content_extend = std::make_unique<char[]>(extension_size);
      memset(content_extend.get(), 0, extension_size);
      vsnprintf(content_extend.get(), extension_size, format, args);
    }
    else {
      memset(content, 0, LOGGER_BUFFER_SIZE);
      vsnprintf(content, LOGGER_BUFFER_SIZE, format, args);
    }
    va_end(args);

    // make header
    struct tm    tm;
    struct timeb tb;
    ftime(&tb);
    char timestamp[24];

    localtime_r(&tb.time, &tm);

    if (raw == false) {
      snprintf(timestamp, sizeof(timestamp),
               "%04d-%02d-%02d %02d:%02d:%02d.%03d", tm.tm_year + 1900,
               tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
               tb.millitm);
    }
    else {
      memset(timestamp, 0, sizeof(timestamp));
    }

    // make tail
    char logging_tail[256];
    memset(logging_tail, '\0', sizeof(logging_tail));

    if (file && line > 0 && raw == false) {
      snprintf(logging_tail, sizeof(logging_tail), "  ... (%s: %d)", file,
               line);
    }

    // make log level key
    const char* keyword;
    switch (level) {
      case Level::FATAL:
        keyword = "[FATAL]";
        break;
      case Level::ERROR:
        keyword = "[ERROR]";
        break;
      case Level::WARN:
        keyword = "[WARN]";
        break;
      case Level::INFO:
        keyword = "[INFO]";
        break;
      case Level::DEBUG:
        keyword = "[DEBUG]";
        break;
      case Level::TRACE:
        keyword = "[TRACE]";
        break;
      default:
        keyword = "";
        break;
    }
    if (raw)
      keyword = "";

    // make content
    char  content_console[LOGGER_BUFFER_SIZE];
    char  content_file[LOGGER_BUFFER_SIZE];
    char* content_console_ext = nullptr;
    char* content_file_ext    = nullptr;
    if (extension_size > 0) {
      content_console_ext = new char[extension_size];
      content_file_ext    = new char[extension_size];
      memset(content_console_ext, 0, extension_size);
      memset(content_file_ext, 0, extension_size);
      if (raw == false) {
        snprintf(content_console_ext, extension_size, "[%s] %-7s %s\n",
                 timestamp, keyword, content_extend.get());
        snprintf(content_file_ext, extension_size, "[%s] %-7s %s %s\n",
                 timestamp, keyword, content_extend.get(), logging_tail);
      }
      else {
        snprintf(content_console_ext, extension_size, "%s\n",
                 content_extend.get());
        snprintf(content_file_ext, extension_size, "%s\n",
                 content_extend.get());
      }
    }
    else {
      memset(content_console, 0, LOGGER_BUFFER_SIZE);
      memset(content_file, 0, LOGGER_BUFFER_SIZE);
      if (raw == false) {
        snprintf(content_console, LOGGER_BUFFER_SIZE, "[%s] %-7s %s\n",
                 timestamp, keyword, content);
        snprintf(content_file, LOGGER_BUFFER_SIZE, "[%s] %-7s %s %s\n",
                 timestamp, keyword, content, logging_tail);
      }
      else {
        snprintf(content_console, LOGGER_BUFFER_SIZE, "%s\n", content);
        snprintf(content_file, LOGGER_BUFFER_SIZE, "%s\n", content);
      }
    }

    ////////////////////////////////////////////////
    if (write_file) {
      char file_dir[15];
      snprintf(file_dir, sizeof(file_dir), "%s/%04d_%02d_%02d",
               LOGGER_FOLDER_TITEL, tm.tm_year + 1900, tm.tm_mon + 1,
               tm.tm_mday);

      if (previous_hour_ != tm.tm_hour) {
        previous_hour_ = tm.tm_hour;
        is_first_file_ = true;
      }

      if (is_first_file_) {
        char file_time[14];
        snprintf(file_time, sizeof(file_time), "%04d_%02d_%02d-%02d",
                 tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour);

        startup_count_ = [file_dir, file_time]() {
          int            file_count = 0;
          DIR*           dirp       = nullptr;
          struct dirent* entry;

          dirp = opendir(file_dir);

          if (dirp == nullptr) {
            mkdir(file_dir, 0777);
            return file_count;
          }

          while ((entry = readdir(dirp)) != NULL) {
            if (entry->d_type == DT_REG) {
              if (strcmp(file_time, entry->d_name) < 0)
                file_count++;
            }
          }
          closedir(dirp);
          return file_count;
        }();

        is_first_file_ = false;
      }

      char filepath[256];
      memset(filepath, 0, sizeof(filepath));
      snprintf(filepath, sizeof(filepath), "%s/%04d_%02d_%02d-%02d-%02d.txt",
               file_dir, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
               tm.tm_hour, startup_count_ + 1);

      FILE* logger_file = nullptr;
      logger_file       = fopen(filepath, "a");

      if (extension_size > 0)
        fprintf(logger_file, "%s", content_file_ext);
      else
        fprintf(logger_file, "%s", content_file);
      fclose(logger_file);
    }

    if (write_console) {
      if (extension_size > 0)
        printf("%s", content_console_ext);
      else
        printf("%s", content_console);
    }

    if (extension_size > 0) {
      delete[] content_console_ext;
      content_console_ext = nullptr;

      delete[] content_file_ext;
      content_file_ext = nullptr;
    }
  }
  catch (std::exception& e) {
    std::cerr << "logger : exception : " << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "logger : exception : undefined" << std::endl;
  }
}

std::string Logger::extractClass(const std::string& pretty_function)
{
  std::string pretty_func = pretty_function;
  removeLambda(pretty_func);

  // find method start point
  size_t method_start = pretty_func.find("(");
  if (method_start == std::string::npos)
    return pretty_func;
  else
    pretty_func = pretty_func.substr(0, method_start + 1);

  size_t colons = pretty_func.rfind("::");
  if (colons == std::string::npos)
    return "(NO CLASS)";
  else {  // include namespace
    size_t colons_prev, begin;
    if ((colons_prev = pretty_func.rfind("::", colons - 1)) ==
        std::string::npos)
      begin = pretty_func.rfind(" ", colons) + 1;
    else
      begin = colons_prev + 2;  // 2: length of "::"
    return pretty_func.substr(begin, colons - begin);
  }
}

std::string Logger::extractMethod(const std::string& pretty_function)
{
  std::string pretty_func = pretty_function;
  removeLambda(pretty_func);

  // find method start point
  size_t method_start = pretty_func.find("(");
  if (method_start == std::string::npos)
    return pretty_func;
  else
    pretty_func = pretty_func.substr(0, method_start + 1);

  size_t colons, begin;
  if ((colons = pretty_func.rfind("::")) == std::string::npos)
    begin = pretty_func.rfind(" ", colons) + 1;
  else
    begin = colons + 2;
  size_t end = pretty_func.rfind("(") - begin;
  return pretty_func.substr(begin, end) + "()";
}

void Logger::removeLambda(std::string& pretty_func)
{
  std::string lambda_key("::<lambda()>");
  size_t      colons = pretty_func.find(lambda_key);
  if (colons != std::string::npos) {
    pretty_func.erase(colons, lambda_key.length());
  }
}
}  // namespace rs