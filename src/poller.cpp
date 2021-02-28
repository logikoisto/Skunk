#include "skunk/poller.h"

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <memory>

#include "skunk/channel.h"
#include "skunk/event_loop.h"
#include "util/condition.h"
#include "util/mutex.h"
namespace zoo {
namespace skunk { /* 轮询器的生命周期由 event loop对象唯一管理*/
Poller::Poller(std::shared_ptr<EventLoop> loop)
    : epoll_fd_(epoll_create(1024)), owner_loop_(loop), events_(16){};
Poller::~Poller() {
  std::vector<std::shared_ptr<Channel>> active_channel_list;
  std::shared_ptr<EventLoop> owner_loop;
  std::vector<struct epoll_event> events;
  std::map<int32_t, std::shared_ptr<Channel>> channel_map;
  owner_loop.swap(owner_loop_);
  channel_map.swap(channel_map_);
  active_channel_list.swap(active_channel_list_);
  events.swap(events_);
  ::close(epoll_fd_);
};

/**
 * 1. 底层epoll_wait返回在 epoll_fd上注册的事件
 * 2. 通过 构建的fd 与 channel之间的索引查找到对应的channel对象
 * 3. 添加到 active channel list 列表中返回
 */
void Poller::Poll(int64_t timeout) {
  int event_count = epoll_wait(epoll_fd_, &*events_.begin(),
                               static_cast<int>(events_.size()), timeout);
  for (int i = 0; i < event_count; i++) {  //遍历所有事件
    std::map<int32_t, std::shared_ptr<Channel>>::iterator iter =
        channel_map_.find(events_[i].data.fd);
    if (iter != channel_map_.end()) {
      iter->second->SetEvents(events_[i].events);
      active_channel_list_.push_back(iter->second);
    } else {
      epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, iter->second->Fd(), nullptr);
    }
  }
  if (static_cast<size_t>(event_count) == events_.size()) {
    events_.resize(events_.size() * 2);
  }
};
/**
 * 对poller持有的channel 进行管理的操作函数
 */
void Poller::UpdateChannel(std::shared_ptr<Channel> channel) {
  std::map<int32_t, std::shared_ptr<Channel>>::iterator iter =
      channel_map_.find(channel->Fd());
  struct epoll_event ev;
  int32_t res;
  ev.events = channel->Events();
  ev.data.fd = channel->Fd();
  if (iter != channel_map_.end()) {
    res = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, channel->Fd(), &ev);
  } else {
    channel_map_[channel->Fd()] = channel;
    res = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, channel->Fd(), &ev);
  }
  if (res < 0) {
    // LOG err
  }
};

void Poller::RemoveChannel(std::shared_ptr<Channel> channel) {
  std::map<int32_t, std::shared_ptr<Channel>>::const_iterator iter;
  iter = channel_map_.find(channel->Fd());
  if (iter != channel_map_.end()) {
    channel_map_.erase(iter);
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, channel->Fd(), nullptr) < 0) {
      // log
    }
  }
};
bool Poller::HasChannel(std::shared_ptr<Channel> channel) const {
  std::map<int32_t, std::shared_ptr<Channel>>::const_iterator iter;
  iter = channel_map_.find(channel->Fd());
  return iter != channel_map_.end();
};
}  // namespace skunk
}  // namespace zoo