#ifndef __ROWEN_SDK_CORE_TIME_HPP__
#define __ROWEN_SDK_CORE_TIME_HPP__

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>

using namespace std::chrono_literals;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::nanoseconds;
using std::chrono::seconds;

namespace rs {

class Time {
  using Tick    = uint64_t;
  using Value   = uint64_t;
  using Format  = std::string;
  using CFormat = const std::string&;

 public:
  enum class Resolution : int {
    SEC,    // Second
    MILLI,  // Milli second
    MICRO,  // Micro second
    NANO    // Nano second
  };

 public:
  //////////////////////////
  // Options & Control
  static void setResolution(Resolution register_t = Resolution::MILLI);
  //////////////////////////

  // get time unit(ms, ns, us etc.)
  static const char* unit();

  // get system tick count
  static Tick tick();

  // set or update start time
  static void setTimer(std::string key);

  // remove start time
  static void removeTimer(std::string key);

  // get elapsed tick count
  static Tick elapse(std::string key);

  // check if the specified time has passed (boolean)
  template <typename __literals>
  static bool elapse(__literals target, Tick start, Tick end = 0)
  {
    if (end == 0)
      end = tick();
    return (end - start > literalToTick(target));
  }

  // get time-string in the specified format
  static Format timeString(CFormat format = "%F %T", Tick tick = 0,
                           bool ext = false);

  // get time-string in the fixed format (YYYY-MM-DD HH:mm:SS)
  static Format timeString(Tick tick, bool ext = false);

  // convert second to current resolution tick count
  static Tick casting(long double second);

  // Converts the current resolution tick count to the specified resolution
  template <typename resolution>
  static Tick casting(Tick time)
  {
    return static_cast<Time::Tick>((time * resolutionTick<resolution>()) /
                                   resolutionTick());
  }

  // running interval functions
  template <typename literals, typename callable, typename... Args>
  static void runInterval(literals interval, std::string key, callable&& func,
                          Args&&... args)
  {
    auto time = tick();
    auto iter = interval_map_.find(key);
    if (iter != interval_map_.end()) {
      if (time - iter->second > literalToTick(interval)) {
        func(std::forward<args>(args)...);
        iter->second = time;
      }
    }
    else {
      interval_map_.insert({ key, time });
    }
  }

  template <typename literals>
  static void sleep(literals time)
  {
    std::this_thread::sleep_for(time);
  }

 private:
  // tick count to string
  static Format tickToString(Tick tick, bool ext, CFormat format);

  template <typename resolution>
  static Format _tickToString(Tick tick, bool ext, CFormat format);

  static long double resolutionTick();

  template <typename resolution>
  static Tick resolutionTick()
  {
    if (std::is_same<resolution, seconds>())
      return 1;
    if (std::is_same<resolution, milliseconds>())
      return 1000;
    if (std::is_same<resolution, microseconds>())
      return 1000000;
    if (std::is_same<resolution, nanoseconds>())
      return 1000000000;
    return 1;
  }

  // get adjust integer value (based on current resolution)
  template <typename literals>
  static Tick literalToTick(literals time)
  {
    using std::chrono::duration_cast;
    switch (current_time_resolution_) {
      case Resolution::SEC:
        return duration_cast<seconds>(time).count();
      case Resolution::MILLI:
        return duration_cast<milliseconds>(time).count();
      case Resolution::MICRO:
        return duration_cast<microseconds>(time).count();
      case Resolution::NANO:
        return duration_cast<nanoseconds>(time).count();
      default:
        return UINT64_MAX;
    }
  }

 private:
  static std::unordered_map<std::string, Tick> elapsed_map_;
  static std::unordered_map<std::string, Tick> interval_map_;
  static Resolution                            current_time_resolution_;
};

};  // namespace rs

#endif  //__ROWEN_SDK_CORE_TIME_HPP__