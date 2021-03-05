#include "tcp_server.h"

#include <memory>

#include "connection.h"
#include "event_loop.h"
#include "socket.h"
namespace zoo {
namespace skunk {
TcpServer::TcpServer(const IpAddress &addr, int32_t num)
    : socket_(Socket::CreateTcp()),
      main_loop_(std::make_shared<EventLoop>(true)),
      num_(num),
      running_(true),
      stop_(mutex_),
      timer_manager_() {
  socket_->Bind(addr);
  main_loop_->Start();
  for (int32_t i = 0; i < num_; i++) {
    io_loop_list_.push_back(std::make_shared<EventLoop>(false));
    io_loop_list_[i]->Start();
  }
  timer_manager_->Start();
}
TcpServer::~TcpServer() {
  // 释放 vector的空间
  {
    std::vector<std::shared_ptr<EventLoop>> tmp;
    tmp.swap(io_loop_list_);
  }
  timer_manager_->Stop();
}
std::shared_ptr<TimerManager> TcpServer::GetTimerManager() {
  return timer_manager_;
};

/* 注册的各种回调函数 */
void TcpServer::RegisterConnectionHandler(ConnectionCallback callback) {
  conn_callback_ = callback;
};
void TcpServer::RegisterMessageHandler(MessageCallback callback) {
  message_callback_ = callback;
};
void TcpServer::RegisterCloseHandler(CloseCallback callback) {
  close_callback_ = callback;
};
void TcpServer::RegisterErrorHandler(ErrorCallback callback) {
  error_callback_ = callback;
};
// 启动 server
void TcpServer::Run() {
  socket_->Listen();
  main_loop_->AddConnection(socket_, EventLoop::Accept);
  stop_.wait();
};
// 关闭Server
void TcpServer::Shutdown() {
  MutexGuard gurd(mutex_);
  if (!running_) return;
  running_ = false;
  main_loop_->Stop();
  for (int32_t i = 0; i < num_; i++) {
    io_loop_list_[i]->Stop();
  }
  stop_.notifyAll();
}
Socket::Ptr TcpServer::Server() const { return socket_; }
int32_t TcpServer::Num() const { return num_; }
std::shared_ptr<EventLoop> TcpServer::GetIOLoopByIdx(int32_t idx) {
  if (idx < 0 || idx > num_) {
    // LOG
    return std::shared_ptr<EventLoop>();
  }
  return io_loop_list_[idx];
};

/* 各种回调函数 */
void TcpServer::ConnectionHandler(const std::shared_ptr<Connection> conn) {
  conn_callback_(conn);
};
void TcpServer::MessageHandler(const std::shared_ptr<Connection> conn) {
  message_callback_(conn);
};
void TcpServer::CloseHandler(const std::shared_ptr<Connection> conn) {
  close_callback_(conn);
};
void TcpServer::ErrorHandler(const std::shared_ptr<Connection> conn) {
  error_callback_(conn);
};
}  // namespace skunk
}  // namespace zoo