#ifndef __SKUNK_TCP_SERVER_H__
#define __SKUNK_TCP_SERVER_H__

#include <functional>
#include <memory>
#include <vector>

#include "skunk/address.h"
#include "skunk/callbacks.h"
#include "skunk/connection.h"
#include "skunk/event_loop.h"
#include "skunk/socket.h"
#include "util/condition.h"
#include "util/mutex.h"
namespace zoo {
namespace skunk {
class Connection;
class EventLoop;
/* 对用户暴露的可操作的类 封装各种回调函数以及配置信息 */
class TcpServer {
 public:
  using Ptr = std::shared_ptr<TcpServer>;
  /* server 对象的生命周期管理 */
  TcpServer(const IpAddress &addr, int32_t num);
  ~TcpServer();
  void Run();
  void Shutdown();
  Socket::Ptr Server() const;
  int32_t Num() const;
  std::shared_ptr<EventLoop> GetIOLoopByIdx(int32_t idx);

  /* 注册的各种回调函数 */
  void RegisterConnectionHandler(ConnectionCallback callback);
  void RegisterMessageHandler(MessageCallback callback);
  void RegisterCloseHandler(CloseCallback callback);
  void RegisterErrorHandler(ErrorCallback callback);

  /* 各种回调函数 */
  void ConnectionHandler(const std::shared_ptr<Connection> conn);
  void MessageHandler(const std::shared_ptr<Connection> conn);
  void CloseHandler(const std::shared_ptr<Connection> conn);
  void ErrorHandler(const std::shared_ptr<Connection> conn);

 private:
  Condition stop_;
  /** running 状态 */
  bool running_;
  /** 私有的锁对象 */
  Mutex mutex_;
  /** 服务端 socket 对象 */
  std::shared_ptr<Socket> socket_;
  /** 主事件循环 **/
  std::shared_ptr<EventLoop> main_loop_;
  /** IO 循环 event loop 的个数*/
  int32_t num_;
  /** IO 循环*/
  std::vector<std::shared_ptr<EventLoop>> io_loop_list_;
  /*当连接建立时执行*/
  ConnectionCallback conn_callback_ =
      [](const std::shared_ptr<Connection>) -> void {};
  /*当socket上有可读事件发生时执行*/
  MessageCallback message_callback_ =
      [](const std::shared_ptr<Connection>) -> void {};
  /*当socket 关闭时执行 */
  CloseCallback close_callback_ =
      [](const std::shared_ptr<Connection>) -> void {};
  /*当 socket处理发生错误时执行 */
  ErrorCallback error_callback_ =
      [](const std::shared_ptr<Connection>) -> void {};
};
}  // namespace skunk
}  // namespace zoo
#endif  // _ZOO_SKUNK_TCP_SERVER_H_