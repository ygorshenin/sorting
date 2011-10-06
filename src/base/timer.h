#ifndef BASE_TIMER_H
#define BASE_TIMER_H

#include <sys/types.h>

#include "base/macros.h"


namespace base {

class Timer {
 public:
  Timer();

  bool Start();

  bool Stop();

  double GetTimeElapsed() const;

 private:
  enum State { NEW, RUNNING, STOPPED };

  State state_;
  clock_t start_time_;
  clock_t stop_time_;

  DISABLE_EVIL_CONSTRUCTORS(Timer);
}; // class Timer

}  // namespace base

#endif // #ifndef BASE_TIMER_H
