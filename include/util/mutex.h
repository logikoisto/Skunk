#ifndef __SKUNK_MUTEX_H__
#define __SKUNK_MUTEX_H__
#include <pthread.h>
namespace zoo {
namespace skunk {
class Mutex {
 public:
  Mutex() { pthread_mutex_init(&mutex_, NULL); }
  ~Mutex() { pthread_mutex_destroy(&mutex_); }

  void lock();
  void unlock();

  pthread_mutex_t *getMutex() { return &mutex_; }

 private:
  pthread_mutex_t mutex_;
};

class MutexGuard {
 public:
  MutexGuard(Mutex &mutex);
  ~MutexGuard();

 private:
  Mutex &mutex_;
};

}  // namespace skunk
}  // namespace zoo
#endif