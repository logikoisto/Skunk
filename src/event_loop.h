#ifndef ZOO_SKUNK_EVENT_LOOP_H_
#define ZOO_SKUNK_EVENT_LOOP_H_

#include <map>
#include <memory>

#include "connection.h"
#include "poller.h"
#include "socket.h"
#include "util/condition.h"
#include "util/thread.h"
namespace zoo {
namespace skunk {
class Connection;
class Socket;
class Channel;
class Poller;
/**
 * 1. 负责管理IO线程的类
 * 2. 在网络库启动时根据配置创建N个IO线程
 * 3. 每个IO线程执行的Loop函数完成事件循环逻辑
 */
class EventLoop : std::enable_shared_from_this<EventLoop> {
 public:
  enum Event {
    Accept,
    Read,
    Write,
  };
  EventLoop(bool is_main_loop);
  ~EventLoop();
  void Start();
  void Stop();
  // TODO: timer api
  static std::shared_ptr<EventLoop> CreateEventLoop(bool isMainLoop);
  void AddConnection(std::shared_ptr<Socket> socket, Event event);
  void RemoveChannel(const std::shared_ptr<Channel> channel);
  void UpdateChannel(const std::shared_ptr<Channel> channel);
  // 标记是否为一个主事件循环
  bool is_main_loop_;

 private:
  void Loop();
  std::shared_ptr<Poller> poller_;
  bool started_;
  Thread thread_;
  Mutex mutex_;
};
}  // namespace skunk
}  // namespace zoo
#endif  // ZOO_SKUNK_EVENT_LOOP_H_
