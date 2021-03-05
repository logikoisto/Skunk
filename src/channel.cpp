

#include "channel.h"

#include <errno.h>
#include <sys/epoll.h>
#include <sys/uio.h>

#include <map>
#include <memory>

#include "address.h"
#include "buffer.h"
#include "event_loop.h"
namespace zoo {
namespace skunk {

Channel::Channel(const std::shared_ptr<EventLoop> loop)
    : event_flag_(0),
      loop_(loop){

      };
Channel::~Channel(){};

uint32_t Channel::Events() const { return event_flag_; };

void Channel::EnableAccept() {
  if (!IsAccept()) {
    event_flag_ |= EPOLLIN;
  }
};
/**
 *  开启与关闭 当前channel所关心的事件集合
 */
void Channel::EnableRead() {
  if (!IsRead()) {
    event_flag_ |= EPOLLIN;
  }
};
void Channel::EnableWrite() {
  if (!IsWrite()) {
    event_flag_ |= EPOLLOUT;
  }
};
void Channel::DisableRead() {
  if (IsRead()) {
    event_flag_ ^= EPOLLIN;
  }
};
void Channel::DisableWrite() {
  if (IsWrite()) {
    event_flag_ ^= EPOLLOUT;
  }
};

void Channel::DisableAll() { event_flag_ = 0; };

void Channel::SetEvents(uint32_t events) { event_flag_ = events; };
/**
 * 用于根据 event_flag_ 确定回调哪个用户函数
 */
void Channel::HandleEvent() {
  if (IsAccept()) {
    ProcessorAccept();
  } else if (IsError()) {
    ProcessorError();
  } else if (IsRead()) {
    ProcessorRead();
  } else if (IsWrite()) {
    ProcessorWrite();
  }
};

/**
 * 判断 事件类型
 */
bool Channel::IsAccept() const { return loop_->is_main_loop_; };
bool Channel::IsRead() const { return event_flag_ & EPOLLIN; };
bool Channel::IsWrite() const { return event_flag_ & EPOLLOUT; };
bool Channel::IsError() const { return event_flag_ & (EPOLLERR | EPOLLHUP); };
/**
 * 调用 tcp server中的回调函数 处理不同的事件逻辑
 */
void Channel::ProcessorAccept() {
  IpAddress peer_addr;
  int32_t connfd = server_->Server()->Accept(peer_addr);
  if (connfd < 0) {
    // log
  }
  Socket::Ptr socket = std::make_shared<Socket>(connfd);
  socket->SetNonBlockAndCloseOnExec();
  int32_t idx = socket->Fd() % server_->Num();
  server_->ConnectionHandler(conn_);
  server_->GetIOLoopByIdx(idx)->AddConnection(socket, EventLoop::Read);
};
void Channel::ProcessorRead() {
  Buffer::Ptr in_buffer = conn_->GetInBuffer();
  char buf[kMassageMaxLen];  // 65535
  ssize_t n = conn_->GetSocket()->Read(&buf, kMassageMaxLen - 1);
  if (n <= 0) {
    return;
  }
  in_buffer->Write(Buffer::Current, &buf, n);
  server_->MessageHandler(conn_);
  this->DisableRead();
};
void Channel::ProcessorWrite() {
  // 将buf里的数据 写入到 socket中 并处理写入的异常
  Buffer::Ptr out_buffer = conn_->GetInBuffer();
  size_t readSize = out_buffer->Readable();
  if (readSize <= 0) {
    return;
  }
  struct iovec *vec;
  size_t vector_size;
  out_buffer->Read(readSize, vec, vector_size);
  if (conn_->GetSocket()->Writev(vec, vector_size) == readSize) {
    out_buffer.reset();
  } else {
    // TODO: 这里读取后 读指针会发生变化吗？
  }
  this->DisableWrite();
};
void Channel::ProcessorError() {
  // 处理错误连接
  server_->ErrorHandler(conn_);
  loop_->RemoveChannel(shared_from_this());
};
int32_t Channel::Fd() const { return this->conn_->GetSocket()->Fd(); };
}  // namespace skunk
}  // namespace zoo