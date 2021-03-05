#ifndef _ZOO_SKUNK_CLIENT_H_
#define _ZOO_SKUNK_CLIENT_H_
#include <string.h>

#include <memory>

#include "address.h"
#include "event_loop.h"
#include "socket.h"
#include "util/mutex.h"
namespace zoo {
namespace skunk {
class TcpClient {
 public:
  /* server 对象的生命周期管理 */
  TcpClient();
  ~TcpClient();
  std::shared_ptr<Connection> Connect(const IpAddress &addr);
  void Disconnect();
  std::shared_ptr<Socket> GetSocket() const;

  /* 注册的各种回调函数 */
  void RegisterMessageHandler(
      std::function<void(const std::shared_ptr<Connection>)> callback);
  void RegisterCloseHandler(
      std::function<void(const std::shared_ptr<Connection>)> callback);
  void RegisterErrorHandler(
      std::function<void(const std::shared_ptr<Connection>)> callback);

  /* 各种回调函数 */
  void MessageHandler(const std::shared_ptr<Connection> conn);
  void CloseHandler(const std::shared_ptr<Connection> conn);
  void ErrorHandler(const std::shared_ptr<Connection> conn);

 private:
  /** running 状态 */
  bool running_;
  /** 私有的锁对象 */
  Mutex mutex_;
  /** 事件循环 **/
  std::shared_ptr<EventLoop> loop_;
  /*当socket上有可读事件发生时执行*/
  std::function<void(const std::shared_ptr<Connection>)> message_callback_ =
      [](const std::shared_ptr<Connection>) -> void {};
  /*当socket 关闭时执行 */
  std::function<void(const std::shared_ptr<Connection>)> close_callback_ =
      [](const std::shared_ptr<Connection>) -> void {};
  /*当 socket处理发生错误时执行 */
  std::function<void(const std::shared_ptr<Connection>)> error_callback_ =
      [](const std::shared_ptr<Connection>) -> void {};
};
}  // namespace skunk
}  // namespace zoo
#endif
