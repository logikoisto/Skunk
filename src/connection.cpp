#include "skunk/connection.h"

#include <fcntl.h>

#include <memory>

#include "skunk/channel.h"
#include "skunk/event_loop.h"
#include "skunk/poller.h"
#include "util/condition.h"
#include "util/mutex.h"
namespace zoo {
namespace skunk {
/* Connection关联 event loop 与 对应的socket */
Connection::Connection(const std::shared_ptr<Channel> channel,
                       const std::shared_ptr<Socket> socket)
    : in_buffer_(std::shared_ptr<Buffer>()),
      out_buffer_(std::shared_ptr<Buffer>()),
      channel_(channel),
      socket_(socket){

      };
Connection::~Connection(){

};
/* 返回已经从socket读取到用户态的数据流 交由用户自己处理二进制的编解码 */
std::shared_ptr<Buffer> Connection::GetInBuffer() { return in_buffer_; };
std::shared_ptr<Buffer> Connection::GetOutBuffer() { return out_buffer_; };
/* 发送数据到底层的buffer中 注册写事件 在 内核socket
 * 可写时将outbuffer内的数据写入内核socket输出缓冲区中 */
size_t Connection::Send(const void* src, size_t len) {
  out_buffer_->Write(Buffer::Current, src, len);
  channel_->EnableWrite();
};
/* 用于获取底层的socket对象 运行用户对  Connection 进行自定义的设置 */
std::shared_ptr<Socket> Connection::GetSocket() { return socket_; };
}  // namespace skunk
}  // namespace zoo