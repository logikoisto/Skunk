#include "skunk/event_loop.h"

#include <fcntl.h>

#include <memory>

#include "skunk/channel.h"
#include "skunk/poller.h"
#include "util/condition.h"
#include "util/mutex.h"
namespace zoo {
namespace skunk {
class Channel;
const int64_t kPollTimeMs = 1000;
EventLoop::EventLoop(bool is_main_loop)
    : thread_(std::bind(&EventLoop::Loop, shared_from_this())),
      poller_(std::make_shared<Poller>(shared_from_this())),
      mutex_(),
      is_main_loop_(is_main_loop),
      started_(false){};
void EventLoop::Start() {
  MutexGuard gurd(mutex_);
  if (started_) {
    return;
  } else {
    thread_.start();
    started_ = true;
  }
};
EventLoop::~EventLoop(){};

void EventLoop::Stop() {
  MutexGuard gurd(mutex_);
  if (!started_) {
    return;
  } else {
    started_ = false;
  }
};

// 事件循环的主流程
void EventLoop::Loop() {
  while (true) {
    {
      MutexGuard gurd(mutex_);
      if (!started_) {
        break;
      }
    }
    poller_->Poll(kPollTimeMs);
    int32_t active_channel_size = poller_->active_channel_list_.size();
    for (int32_t i = 0; i < active_channel_size; i++) {
      // 分发事件
      std::shared_ptr<Channel> channel = poller_->active_channel_list_[i];
      channel->HandleEvent();
    }
    poller_->active_channel_list_.clear();
  }
  thread_.join();
};
// 将连接添加到 当前的 event loop中
void EventLoop::AddConnection(std::shared_ptr<Socket> socket,
                              EventLoop::Event event) {
  // 创建 channel 对象
  std::shared_ptr<Channel> channel =
      std::make_shared<Channel>(shared_from_this());
  std::shared_ptr<Connection> conn =
      std::make_shared<Connection>(channel, socket);
  if (!poller_->HasChannel(channel)) {
    switch (event) {
      case EventLoop::Accept:
        channel->EnableAccept();
        break;
      case EventLoop::Read:
        channel->EnableRead();
        break;
      case EventLoop::Write:
        channel->EnableWrite();
        break;
      default:
        break;
    }
    poller_->UpdateChannel(channel);
  }
};
void EventLoop::RemoveChannel(const std::shared_ptr<Channel> channel) {
  if (poller_->HasChannel(channel)) {
    channel->DisableAll();
    poller_->RemoveChannel(channel);
  }
};
// TODO: timer api
static std::shared_ptr<EventLoop> CreateEventLoop(bool isMainLoop) {
  return std::make_shared<EventLoop>(isMainLoop);
}

}  // namespace skunk
}  // namespace zoo