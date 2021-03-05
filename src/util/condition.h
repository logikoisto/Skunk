#ifndef _ZOO_SKUNK_CONDITION_H_
#define _ZOO_SKUNK_CONDITION_H_
#include <pthread.h>

#include "mutex.h"
#include "noncopyable.h"
namespace zoo {
namespace skunk {
class Condition : public Noncopyable {
 public:
  explicit Condition(Mutex& mutex);
  ~Condition();

  void wait();
  bool wait_seconds(time_t seconds);
  void notify();
  void notifyAll();

 private:
  Mutex& mutex_;
  pthread_cond_t cond_;
};
}  // namespace skunk
}  // namespace zoo
#endif