#ifndef __SKUNK_TIMER_H__
#define __SKUNK_TIMER_H__

#include <fcntl.h>
#include <stdint.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include <atomic>
#include <set>
#include <string>

#include "callbacks.h"
#include "util/mutex.h"
#include "util/thread.h"
namespace zoo {
namespace skunk {

// 时间戳的定义以及符号重定向
class Timestamp {
 public:
  const static uint64_t kMicrosecondsPerSecond = 1000 * 1000;

  Timestamp() : microseconds_from_epoch_(0) {}
  explicit Timestamp(uint64_t microseconds_from_epoch)
      : microseconds_from_epoch_(microseconds_from_epoch) {}

  uint64_t getMicroSecondsFromEpoch() { return microseconds_from_epoch_; }
  time_t getSec() const;
  suseconds_t getUsec() const;
  static Timestamp now();

 private:
  uint64_t microseconds_from_epoch_;
};

inline bool operator<(Timestamp lhs, Timestamp rhs) {
  return lhs.getMicroSecondsFromEpoch() < rhs.getMicroSecondsFromEpoch();
}

inline bool operator==(Timestamp lhs, Timestamp rhs) {
  return lhs.getMicroSecondsFromEpoch() == rhs.getMicroSecondsFromEpoch();
}

inline Timestamp operator+(Timestamp lhs, uint64_t micro_seconds) {
  return Timestamp(lhs.getMicroSecondsFromEpoch() + micro_seconds);
}

inline int64_t operator-(Timestamp lhs, Timestamp rhs) {
  return lhs.getMicroSecondsFromEpoch() - rhs.getMicroSecondsFromEpoch();
}

inline std::ostream& operator<<(std::ostream& os, const Timestamp& timestamp) {
  char buf[50];
  time_t sec = timestamp.getSec();
  struct tm tm;
  localtime_r(&sec, &tm);
  strftime(buf, sizeof buf, "%Y-%m-%d %H:%M:%S", &tm);
  os << std::string(buf);
  return os;
}

// timer 对象的定义
class Timer {
 public:
  Timer(Timestamp timestamp, std::function<void()> cb, uint64_t interval)
      : timestamp_(timestamp),
        interval_(interval),
        sequence_(s_sequence_creator_++),
        cb_(cb) {}

  void setTimestamp(Timestamp timestamp) { timestamp_ = timestamp; }
  Timestamp getTimestamp() { return timestamp_; }
  uint64_t getInterval() { return interval_; };
  int64_t getSequence() { return sequence_; };
  std::function<void()> cb_;

 private:
  Timestamp timestamp_;
  uint64_t interval_;
  int64_t sequence_;
  static std::atomic<int64_t> s_sequence_creator_;
};

// 创建一个 timer fd
int createTimerFd();

// timer 管理器
class TimerManager : std::enable_shared_from_this<TimerManager> {
 public:
  TimerManager()
      : timer_fd_(createTimerFd()),
        thread_(std::bind(&TimerManager::loop, shared_from_this())),
        started_(false) {}
  ~TimerManager() { ::close(timer_fd_); }
  void Start();
  void Stop();
  int64_t addTimer(Timestamp timestamp, std::function<void()> cb,
                   uint64_t interval);
  void cancel(int64_t);

 private:
  bool started_;
  bool findFirstTimestamp(const Timestamp&, Timestamp&);
  ssize_t readTimerFd();
  void resetTimerFd(Timestamp when);
  void dealWithExpiredTimer();
  void loop();
  int timer_fd_;
  std::multimap<Timestamp, std::shared_ptr<Timer>> timer_map_;
  Mutex mutex_;
  Thread thread_;
  // for cancel
  std::map<int64_t, Timestamp> sequence_2_timestamp_;
  std::set<int64_t> cancel_set_;
};
}  // namespace skunk

}  // namespace zoo

#endif  // !__SKUNK_TIMER_H__