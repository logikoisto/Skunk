#ifndef ZOO_SKUNK_EVENT_LOOP_H_
#define ZOO_SKUNK_EVENT_LOOP_H_

#include <map>
#include <memory>

#include "poller.h"
#include "thread"
namespace zoo {
namespace skunk {
/**
 * 1. 负责管理IO线程的类
 * 2. 在网络库启动时根据配置创建N个IO线程
 * 3. 每个IO线程执行的Loop函数完成事件循环逻辑
 */
class EventLoop final {
 public:
  using Ptr = std::shared_ptr<EventLoop>;
  EventLoop();
  ~EventLoop();
  void Loop();
  void Quit();
  // TODO: timer api

 private:
  Poller::Ptr poller_;
  // TODO :....
};
}  // namespace skunk
}  // namespace zoo
#endif  // ZOO_SKUNK_EVENT_LOOP_H_
