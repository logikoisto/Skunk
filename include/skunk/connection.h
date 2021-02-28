#ifndef _ZOO_SKUNK_CONNECTION_H_
#define _ZOO_SKUNK_CONNECTION_H_

#include <memory>
#include <string>

#include "skunk/buffer.h"
#include "skunk/channel.h"
#include "skunk/event_loop.h"
#include "skunk/socket.h"
#include "skunk/tcp_server.h"

namespace zoo {
namespace skunk {
/* 封装对底层socket的读写,维护与远端建立的会话 */
const size_t kMassageMaxLen = 65535;
class TcpServer;
class Socket;
class Channel;
class Connection {
 public:
  /* Connection关联 channel 与 对应的socket */
  Connection(const std::shared_ptr<Channel> channel, const Socket::Ptr socket);
  ~Connection();
  /* 返回已经从socket读取到用户态的数据流 交由用户自己处理二进制的编解码 */
  std::shared_ptr<Buffer> GetInBuffer();
  std::shared_ptr<Buffer> GetOutBuffer();
  /* 发送数据到底层的buffer中 注册写事件 在 内核socket
   * 可写时将outbuffer内的数据写入内核socket输出缓冲区中 */
  size_t Send(const void* src, size_t len);
  /* 用于获取底层的socket对象 运行用户对  Connection 进行自定义的设置 */
  std::shared_ptr<Socket> GetSocket();
  std::shared_ptr<TcpServer> GetTcpServer() const;

 private:
  /* 输入与输出buffer的对象 ring buffer 实现 */
  // TODO: 使用智能指针是否更好?
  std::shared_ptr<Buffer> in_buffer_;
  std::shared_ptr<Buffer> out_buffer_;
  /* 底层的 socket 对象 持有fd句柄 */
  std::shared_ptr<Socket> socket_;
  /* 封装对事件异步的通知  Connection 对象会被用户操作
   * 写入数据时向其注册写事件*/
  std::shared_ptr<Channel> channel_;
};
}  // namespace skunk
}  // namespace zoo
#endif