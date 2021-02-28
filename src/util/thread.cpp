#include "util/thread.h"

#include <assert.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <atomic>

namespace zoo {
namespace skunk {
std::atomic<int> threadCount(0);
static __thread pid_t t_tid = 0;

pid_t Thread::CurrentThreadTid() {
  if (t_tid == 0) {
    t_tid = ::syscall(SYS_gettid);
  }
  return t_tid;
}

Thread::Thread(Func cb, std::string name)
    : started_(false), joined_(false), cb_(std::move(cb)) {
  if (name.empty()) {
    int num = threadCount.fetch_add(1);
    char buf[30];
    snprintf(buf, sizeof buf, "Thread-%d", num);
    name_ = buf;
  } else {
    name_ = name;
  }
}

Thread::~Thread() {
  if (started_ && !joined_) {
    pthread_detach(tid_);
  }
}

bool Thread::isStarted() { return started_; }

void Thread::start() {
  assert(!started_);
  started_ = true;
  if (int error =
          pthread_create(&tid_, nullptr, Thread::threadFuncInternal, this)) {
    started_ = false;
    // log
  }
}

void Thread::join() {
  assert(started_);
  assert(!joined_);
  joined_ = true;
  if (int error = pthread_join(tid_, nullptr)) {
    joined_ = false;
    // log
  }
}

const std::string& Thread::getName() const { return name_; }

void* Thread::threadFuncInternal(void* arg) {
  Thread* thread = static_cast<Thread*>(arg);
  Func cb;
  cb.swap(thread->cb_);

  // todo: exception
  cb();
  return 0;
}
}  // namespace skunk

}  // namespace zoo
