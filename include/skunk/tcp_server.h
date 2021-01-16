#ifndef __SKUNK_TCP_SERVER_H__
#define __SKUNK_TCP_SERVER_H__

#include <functional>
#include <memory>

#include "address.h"
#include "callbacks.h"
#include "connection.h"
namespace zoo {
namespace skunk {
/* 对用户暴露的可操作的类 封装各种回调函数以及配置信息 */
class TcpServer {
 public:
  using Ptr = std::shared_ptr<TcpServer>;
  /* server 对象的生命周期管理 */
  TcpServer(const IpAddress &addr);
  ~TcpServer();
  void AsyncStart();
  void Wait();
  void Shutdown();

  /* 注册的各种回调函数 */
  void RegisterConnectionHandler(ConnectionCallback callback);
  void RegisterMessageHandler(MessageCallback callback);
  void RegisterCloseHandler(CloseCallback callback);
  void RegisterErrorHandler(ErrorCallback callback);

 private:
  /*当连接建立时执行*/
  ConnectionCallback conn_callback_ = [&](const Connection &coon) -> void {};
  /*当socket上有可读事件发生时执行*/
  MessageCallback message_callback_ = [&](const Connection &coon) -> void {};
  /*当socket 关闭时执行 */
  CloseCallback close_callback_ = [&](const Connection &coon) -> void {};
  /*当 socket处理发生错误时执行 */
  ErrorCallback error_callback_ = [&](const Connection &coon) -> void {};
};
}  // namespace skunk
}  // namespace zoo
#endif  // _ZOO_SKUNK_TCP_SERVER_H_