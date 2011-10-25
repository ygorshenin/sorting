#ifndef BASE_TIMER_H
#define BASE_TIMER_H

#include "boost/chrono.hpp"


namespace base {

class Timer {
 public:
  Timer();

  void Restart();

  double Elapsed() const;

 private:
  boost::chrono::system_clock::time_point start_time_;
}; // class Timer

}  // namespace base

#endif // #ifndef BASE_TIMER_H
