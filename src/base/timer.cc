#include <cassert>
#include <ctime>

#include "base/timer.h"

namespace base {

Timer::Timer(): state_(NEW) {}

bool Timer::Start() {
  if (state_ == NEW) {
    state_ = RUNNING;
    start_time_ = clock();
    return true;
  } else
    return false;
}

bool Timer::Stop() {
  if (state_ == RUNNING) {
    state_ = STOPPED;
    stop_time_ = clock();
    return true;
  } else
    return false;
}

double Timer::GetTimeElapsed() const {
  if (state_ == STOPPED)
    return static_cast<double>(stop_time_ - start_time_) /
      static_cast<double>(CLOCKS_PER_SEC);
  else
    return -1.0;
}

}  // namespace base
