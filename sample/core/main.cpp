#include <iostream>

#include "rowen/core.hpp"

int main()
{
  std::string str;

  ////////////////////////////////////////////////////////////////////////////
  // core functions
  str = rs::format("format sample: %s %d %.3f", "rexgen", 123, 456.789);
  std::cout << str << std::endl;

  ////////////////////////////////////////////////////////////////////////////
  // Logger

  // initialize
  rs::Logger::setup(rs::Logger::Level::DEBUG, true);

  // folder change
  rs::Logger::setDirectory("change_log_folder1");
  logger.info("change log folder test 1");

  rs::Logger::resetDirectory();
  logger.info("reset log folder test 1");

  rs::Logger::setDirectory("change_log_folder2");
  logger.info("change log folder test 2");

  rs::Logger::resetDirectory();
  logger.info("reset log folder test 2");


  // Method (all ok)
  logger.info("this is log %d", 1);       // default
  rs::Logger::info("this is log %d", 2);  // also same above
  logger_info("this is log %d", 3);       // include file & line

  // trace log is not visible in console, only in the file
  logger.trace("this is trace log");

  // parsing class or current function's name
  logger.info("%s : this is class", __CLASS__);
  logger.info("%s : this is method", __METHOD__);

return 0;
  ////////////////////////////////////////////////////////////////////////////
  // Time
  using rs::Time;

  // setup (setup time resolution)
  Time::setup(Time::Resolution::MILLI);  // or
  Time::setup(Time::Resolution::MICRO);  // or
  Time::setup(Time::Resolution::NANO);

  // ex 1. Get tick count
  auto tick = Time::tick();

  // ex 2. Stop-watch
  Time::setTimer("my-stop watch-1");
  Time::setTimer("my-stop watch-2");

  Time::sleep(1s);
  auto elapsed1 = Time::elapse("my-stop watch-1");
  logger.info("elapsed 1 : %lu %s", elapsed1, Time::unit());

  Time::sleep(2s);
  auto elapsed2 = Time::elapse("my-stop watch-2");
  logger.info("elapsed 2 : %lu %s", elapsed2, Time::unit());

  // ex 3. Time string
  auto tick_time_string     = Time::timeString(tick);
  auto curr_time_string     = Time::timeString();
  auto tick_time_format_str = Time::timeString("%Y%m%d_%H%M%S", tick);
  auto curr_time_format_str = Time::timeString("%Y%m%d_%H%M%S");
  auto time_string_ext      = Time::timeString(tick, true);  // with millisec

  std::cout << tick_time_string << std::endl;
  std::cout << curr_time_string << std::endl;
  std::cout << tick_time_format_str << std::endl;
  std::cout << curr_time_format_str << std::endl;
  std::cout << time_string_ext << std::endl;

  // ex 4. Get current month or hour (using Time string)
  auto month = std::stoi(Time::timeString("%m"));
  auto hour  = std::stoi(Time::timeString("%H"));
  printf("current month: %d, hour: %d\n", month, hour);

  // ex 5. Time elased checking (500ms)
  auto start_time = Time::tick();
  printf("Elapsed 500ms ?  %s\n",
         Time::elapse(500ms, start_time) ? "YES" : "NO");
  Time::sleep(500ms);
  printf("Elapsed 500ms ?  %s\n",
         Time::elapse(500ms, start_time) ? "YES" : "NO");

  // ex 6. tick count casting (if nano-seconds)
  {
    auto elapsed_time = Time::tick() - tick;
    printf("origin time : %llu\n", elapsed_time);

    // case 1. second to "nano-second"
    auto tick = Time::casting(3);  // 3 sec is "N" nano seconds
    printf("3 second is %llu nano-seconds\n", tick);

    // case 2. "nano-second" to second
    auto to_sec = Time::casting<seconds>(elapsed_time);
    printf("nano to sec : %llu\n", to_sec);

    // case 3. "nano-second" to milli-second
    auto to_ms = Time::casting<milliseconds>(elapsed_time);
    printf("nano to milli : %llu\n", to_ms);
  }

  // ex 7. interval function
  auto start = Time::tick();
  while (Time::elapse(5s, start) == false) {
    Time::runInterval(1s, "interval_1sec",
                      [&] { logger.info("run interval 1s"); });

    Time::runInterval(500ms, "interval_500ms_1",
                      [&] { logger.info("run interval 500ms"); });

    Time::runInterval(500000000ns, "interval_500ms_2",
                      [&] { logger.info("run interval 500000000ns"); });
  }

  return 0;
}