#ifndef __ZOO_SKUNK_POLLER_H__
#define __ZOO_SKUNK_POLLER_H__

#include <sys/epoll.h>

#include <map>
#include <memory>
#include <vector>

#include "channel.h"
#include "event_loop.h"
namespace zoo {
namespace skunk {
class EventLoop;
class Channel;
/**
 * poller 对象封装对底层IO复用模型的操作 提供poll语义
 * 1. 向poller对象注册所关心的fd以及其上发生的事件
 * 2. 通过poll函数返回内核通知的发生事件的fd列表
 * 3. 根据被激活的fd找到对应的channel对象调用其事件处理函数
 * 4. channel对象正确调用用户注册的回调函数处理相应的事件
 */
class Poller {
 public:
  /* 轮询器的生命周期由 event loop对象唯一管理*/
  Poller(std::shared_ptr<EventLoop> loop);
  ~Poller();
  /* 定义一个成员对象 用于装载内核通知的被激活fd对应的channel */
  std::vector<std::shared_ptr<Channel>> active_channel_list_;
  /**
   * 1. 底层epoll_wait返回在 epoll_fd上注册的事件
   * 2. 通过 构建的fd 与 channel之间的索引查找到对应的channel对象
   * 3. 添加到 active channel list 列表中返回
   */
  void Poll(int64_t timeout);

  /**
   * 对poller持有的channel 进行管理的操作函数
   */
  void UpdateChannel(std::shared_ptr<Channel> channel);
  void RemoveChannel(std::shared_ptr<Channel> channel);
  bool HasChannel(std::shared_ptr<Channel> channel) const;

 private:
  /* 内核中epoll对象的 文件句柄 */
  int32_t epoll_fd_;
  /* 从属的 event loop对象 */
  std::shared_ptr<EventLoop> owner_loop_;
  /* 用于通过fd找到对应channel的红黑树索引*/
  std::map<int32_t, std::shared_ptr<Channel>> channel_map_;
  std::vector<struct epoll_event> events_;
};
}  // namespace skunk
}  // namespace zoo
#endif  // __ZOO_SKUNK_POLLER_H_
