#include "base/timer.h"


namespace base {

Timer::Timer(): start_time_(boost::chrono::system_clock::now()) {
}

void Timer::Restart() {
  start_time_ = boost::chrono::system_clock::now();
}

double Timer::Elapsed() const {
  boost::chrono::system_clock::time_point end_time =
    boost::chrono::system_clock::now();
  return boost::chrono::duration<double>(end_time - start_time_).count();
}

}  // namespace base
