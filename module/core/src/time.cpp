#include "time.hpp"

#include <cmath>
#include <iostream>

using std::chrono::duration_cast;
using std::chrono::hours;
using std::chrono::system_clock;

namespace rs {

std::unordered_map<std::string, Time::Tick> Time::elapsed_map_;
std::unordered_map<std::string, Time::Tick> Time::interval_map_;
Time::Resolution Time::current_time_resolution_ = Time::Resolution::MILLI;

////////////////////////////////////////////////////////////////////////////////
// Options & Control

// clang-format off
void Time::setResolution(Resolution resolution) { current_time_resolution_ = resolution; }
// clang-format on

////////////////////////////////////////////////////////////////////////////////

const char* Time::unit()
{
  switch (current_time_resolution_) {
    case Resolution::SEC:
      return "sec";
    case Resolution::MILLI:
      return "ms";
    case Resolution::MICRO:
      return "us";
    case Resolution::NANO:
      return "ns";
  }
  return "null";
}

Time::Tick Time::tick()
{
  auto t = system_clock::now().time_since_epoch();
  switch (current_time_resolution_) {
    case Resolution::SEC:
      return static_cast<Time::Tick>(duration_cast<seconds>(t).count());
    case Resolution::MILLI:
      return static_cast<Time::Tick>(duration_cast<milliseconds>(t).count());
    case Resolution::MICRO:
      return static_cast<Time::Tick>(duration_cast<microseconds>(t).count());
    case Resolution::NANO:
      return static_cast<Time::Tick>(duration_cast<nanoseconds>(t).count());
  }
  return 0;
}

void Time::setTimer(std::string key)
{
  auto iter = elapsed_map_.find(key);
  if (iter != elapsed_map_.end())
    iter->second = tick();
  else
    elapsed_map_.insert({ key, tick() });
}

void Time::removeTimer(std::string key)
{
  auto iter = elapsed_map_.find(key);
  if (iter != elapsed_map_.end())
    elapsed_map_.erase(iter);
}

Time::Tick Time::elapse(std::string key)
{
  auto iter = elapsed_map_.find(key);
  if (iter != elapsed_map_.end())
    return (tick() - iter->second);
  return 0;
}

Time::Format Time::timeString(CFormat format, Tick tick, bool ext)
{
  return tickToString(tick, ext, format);
}

Time::Format Time::timeString(Tick tick, bool ext)
{
  return tickToString(tick, ext, "%F %T");
}

Time::Tick Time::casting(long double second)
{
  auto tickValue = second * resolutionTick();
  return static_cast<Time::Tick>(std::round(tickValue));
}

Time::Format Time::tickToString(Tick tick, bool ext, CFormat format)
{
  if (tick == 0)
    tick = Time::tick();

  switch (current_time_resolution_) {
    case Resolution::SEC:
      return _tickToString<seconds>(tick, ext, format);
    case Resolution::MILLI:
      return _tickToString<milliseconds>(tick, ext, format);
    case Resolution::MICRO:
      return _tickToString<microseconds>(tick, ext, format);
    case Resolution::NANO:
      return _tickToString<nanoseconds>(tick, ext, format);
  }
  return "";
}

template <typename __resolution>
std::string Time::_tickToString(Tick tick, bool ext, CFormat format)
{
  using namespace std;
  using time_point = system_clock::time_point;
  using t_duration = time_point::duration;

  time_point  tp{ duration_cast<t_duration>(__resolution(tick)) };
  std::time_t t = system_clock::to_time_t(tp);

  std::ostringstream oss;
  struct tm          bt;

#ifdef _WIN32
  localtime_s(&bt, &t);
#else
  localtime_r(&t, &bt);
#endif
  oss << std::put_time(&bt, format.c_str());

  if (ext) {
    if (std::is_same<__resolution, seconds>())
      oss << '.' << setfill('0') << setw(3) << (tick % 1);
    else if (std::is_same<__resolution, milliseconds>())
      oss << '.' << setfill('0') << setw(3) << (tick % 1000);
    else if (std::is_same<__resolution, microseconds>())
      oss << '.' << setfill('0') << setw(3) << (tick % 1000000 / 1000);
    else if (std::is_same<__resolution, nanoseconds>())
      oss << '.' << setfill('0') << setw(3) << (tick % 1000000000 / 1000000);
  }

  return oss.str();
}

long double Time::resolutionTick()
{
  switch (current_time_resolution_) {
    case Resolution::SEC:
      return 1.0L;
    case Resolution::MILLI:
      return 1000.0L;
    case Resolution::MICRO:
      return 1000000.0L;
    case Resolution::NANO:
      return 1000000000.0L;
    default:
      return 1.0L;
  }
}

}  // namespace rs