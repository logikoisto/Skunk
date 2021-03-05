#include "timer.h"

#include <assert.h>
#include <sys/time.h>

#include <atomic>
namespace zoo {
namespace skunk {
std::atomic<int64_t> Timer::s_sequence_creator_;

Timestamp Timestamp::now() {
  struct timeval tv;
  if (gettimeofday(&tv, nullptr)) {
    // log
  }
  return Timestamp(tv.tv_sec * kMicrosecondsPerSecond + tv.tv_usec);
}

time_t Timestamp::getSec() const {
  return microseconds_from_epoch_ / kMicrosecondsPerSecond;
}

suseconds_t Timestamp::getUsec() const {
  return microseconds_from_epoch_ % kMicrosecondsPerSecond;
}

int32_t createTimerFd() {
  int32_t timerfd =
      ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

  if (timerfd < 0) {
    // log
  }
  return timerfd;
}

bool TimerManager::findFirstTimestamp(const Timestamp& now,
                                      Timestamp& timestamp) {
  for (const auto& pair : timer_map_) {
    if (now < pair.first) {
      timestamp = pair.first;
      return true;
    }
  }
  return false;
}

void TimerManager::Start() {
  MutexGuard gurd(mutex_);
  if (started_) {
    return;
  } else {
    thread_.start();
    started_ = true;
  }
};
void TimerManager::Stop() {
  MutexGuard gurd(mutex_);
  if (!started_) {
    return;
  } else {
    started_ = false;
  }
};

int64_t TimerManager::addTimer(Timestamp when, std::function<void()> cb,
                               uint64_t interval) {
  std::shared_ptr<Timer> timer = std::make_shared<Timer>(when, cb, interval);
  bool earliest_timer_changed = false;
  {
    MutexGuard lock(mutex_);
    auto it = timer_map_.begin();
    if (it == timer_map_.end() || when < it->first) {
      earliest_timer_changed = true;
    }
    timer_map_.insert({when, timer});
    sequence_2_timestamp_.insert({timer->getSequence(), when});

    if (earliest_timer_changed) {
      resetTimerFd(when);
    }
  }
  return timer->getSequence();
}

void TimerManager::cancel(int64_t timer_id) {
  {
    MutexGuard lock(mutex_);
    auto it = sequence_2_timestamp_.find(timer_id);
    if (it != sequence_2_timestamp_.end()) {
      timer_map_.erase(it->second);
      sequence_2_timestamp_.erase(it);
    } else {
      cancel_set_.insert(timer_id);
    }
  }
}

void TimerManager::resetTimerFd(Timestamp when) {
  struct itimerspec new_value;
  bzero(&new_value, sizeof(new_value));

  uint64_t micro_seconds_diff = when.getMicroSecondsFromEpoch() -
                                Timestamp::now().getMicroSecondsFromEpoch();
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(micro_seconds_diff /
                                  Timestamp::kMicrosecondsPerSecond);
  ts.tv_nsec = static_cast<long>(micro_seconds_diff %
                                 Timestamp::kMicrosecondsPerSecond * 1000);

  new_value.it_value = ts;
  if (timerfd_settime(timer_fd_, 0, &new_value, nullptr)) {
    // Log
  }
}

ssize_t TimerManager::readTimerFd() {
  uint64_t num_of_expirations;
  ssize_t n = ::read(timer_fd_, &num_of_expirations, sizeof(uint64_t));
  if (n != sizeof num_of_expirations) {
    // log
  }
  return n;
}
void TimerManager::loop() {
  while (started_) {
    {
      MutexGuard gurd(mutex_);
      if (!started_) {
        break;
      }
    }
    dealWithExpiredTimer();
  }
  thread_.join();
}
void TimerManager::dealWithExpiredTimer() {
  readTimerFd();
  std::vector<std::pair<Timestamp, std::shared_ptr<Timer>>> expired;
  {
    MutexGuard lock(mutex_);
    auto it_not_less_now = timer_map_.lower_bound(Timestamp::now());
    std::copy(timer_map_.begin(), it_not_less_now, back_inserter(expired));
    timer_map_.erase(timer_map_.begin(), it_not_less_now);
    for (auto& pair : expired) {
      sequence_2_timestamp_.erase(pair.second->getSequence());
    }
  }

  for (const std::pair<Timestamp, std::shared_ptr<Timer>>& pair : expired) {
    std::shared_ptr<Timer> old_timer = pair.second;
    {
      MutexGuard lock(mutex_);
      if (cancel_set_.find(old_timer->getSequence()) != cancel_set_.end()) {
        continue;
      }
    }
    old_timer->cb_();
    if (old_timer->getInterval() > 0) {
      Timestamp new_timestamp = Timestamp::now() + old_timer->getInterval();
      old_timer->setTimestamp(new_timestamp);
      {
        MutexGuard lock(mutex_);
        timer_map_.insert({new_timestamp, old_timer});
        sequence_2_timestamp_[old_timer->getSequence()] = new_timestamp;
      }
    }
  }
  {
    MutexGuard lock(mutex_);
    cancel_set_.clear();
  }

  Timestamp timestamp;
  if (findFirstTimestamp(Timestamp::now(), timestamp)) {
    resetTimerFd(timestamp);
  }
}

}  // namespace skunk
}  // namespace zoo