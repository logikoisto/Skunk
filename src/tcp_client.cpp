
#include "tcp_client.h"

#include "socket.h"
#include "util/mutex.h"
namespace zoo {
namespace skunk {
TcpClient::TcpClient()
    : loop_(EventLoop::CreateEventLoop(true)), running_(true) {
  loop_->Start();
}
TcpClient::~TcpClient() {}

/* 注册的各种回调函数 */
void TcpClient::RegisterMessageHandler(MessageCallback callback) {
  message_callback_ = callback;
};
void TcpClient::RegisterCloseHandler(CloseCallback callback) {
  close_callback_ = callback;
};
void TcpClient::RegisterErrorHandler(ErrorCallback callback) {
  error_callback_ = callback;
};
// 连接 握手
std::shared_ptr<Connection> TcpClient::Connect(const IpAddress &addr) {
  std::shared_ptr<Socket> socket = Socket::CreateTcp();
  socket->Connect(addr);
  std::shared_ptr<Channel> channel = std::make_shared<Channel>(loop_);
  std::shared_ptr<Connection> conn =
      std::make_shared<Connection>(channel, socket);
  channel->EnableRead();
  loop_->UpdateChannel(channel);
  return conn;
};
// 关闭Server
void TcpClient::Disconnect() {
  MutexGuard gurd(mutex_);
  if (!running_) return;
  running_ = false;
  loop_->Stop();
};
/* 各种回调函数 */
void TcpClient::MessageHandler(const std::shared_ptr<Connection> conn) {
  message_callback_(conn);
};
void TcpClient::CloseHandler(const std::shared_ptr<Connection> conn) {
  close_callback_(conn);
};
void TcpClient::ErrorHandler(const std::shared_ptr<Connection> conn) {
  error_callback_(conn);
};
}  // namespace skunk
}  // namespace zoo