#ifndef __ROWEN_SDK_CORE_FUNCTION_HPP__
#define __ROWEN_SDK_CORE_FUNCTION_HPP__

#include <cstdarg>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

namespace rs {  // rowen sdk

inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v")
{
  s.erase(0, s.find_first_not_of(t));
  return s;
}
inline std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v")
{
  s.erase(s.find_last_not_of(t) + 1);
  return s;
}
inline std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
{
  return ltrim(rtrim(s, t), t);
}

// string
inline const std::string format(const char* fmt, ...)
{
  va_list va_args;
  va_start(va_args, fmt);
  va_list va_args_copy;
  va_copy(va_args_copy, va_args);
  const int result_length = std::vsnprintf(NULL, 0, fmt, va_args_copy);
  va_end(va_args_copy);
  std::vector<char> result(result_length + 1);
  std::vsnprintf(result.data(), result.size(), fmt, va_args);
  va_end(va_args);
  return std::string(result.data(), result_length);
}

// // command
// template <typename... Args>
// const std::string terminal(bool sudo, const char* fmt, Args... args)
// {
//   std::string cmd = format(fmt, args...);
//   if (sudo)
//     cmd = format("echo '1' | sudo -S %s", cmd.c_str());
//   // printf("%s\n", cmd.c_str());
//   std::string result;
//   char        buffer[1024];
//   memset(buffer, 0, sizeof(buffer));
//   cmd.append(" 2>&1");
//   FILE* stream = popen(cmd.c_str(), "r");
//   if (stream) {
//     while (fgets(buffer, sizeof(buffer), stream) != NULL) result.append(buffer);
//     pclose(stream);
//   }
//   return result;
// }

// // atomic alias
// template <typename T>
// T get_atomic(std::atomic<T>& _var)
// {
//   return _var.load(std::memory_order_relaxed);
// }
// template <typename T>
// void set_atomic(std::atomic<T>& _var, T _set)
// {
//   _var.store(_set, std::memory_order_relaxed);
// }
};  // namespace rs

#endif  //__ROWEN_SDK_CORE_FUNCTION_HPP__