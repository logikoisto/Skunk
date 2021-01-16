#ifndef _ZOO_SKUNK_CONNECTION_H_
#define _ZOO_SKUNK_CONNECTION_H_

#include <memory>
#include <string>

#include "buffer.h"
#include "channel.h"
#include "event_loop.h"
#include "socket.h"
#include "tcp_server.h"

namespace zoo {
namespace skunk {
/* 封装对底层socket的读写,维护与远端建立的会话 */
class Connection {
 public:
  using Ptr = std::shared_ptr<Connection>;
  /* Connection关联 event loop 与 对应的socket */
  Connection(const EventLoop &loop, const IpAddress &addr);
  ~Connection();
  /* 返回已经从socket读取到用户态的数据流 交由用户自己处理二进制的编解码 */
  const Buffer &GetBuffer();
  /* 发送数据到底层的buffer中 注册写事件 在 内核socket
   * 可写时将outbuffer内的数据写入内核socket输出缓冲区中 */
  size_t Send(const char *buf, int32_t len);
  /* 用于获取底层的socket对象 运行用户对  Connection 进行自定义的设置 */
  const Socket &GetSocket();

 private:
  /* 输入与输出buffer的对象 ring buffer 实现 */
  // TODO: 使用智能指针是否更好?
  Buffer in_buffer_;
  Buffer out_buffer_;
  /* 底层的 socket 对象 持有fd句柄 */
  Socket::Ptr socket_;
  /* 封装对事件异步的通知  Connection 对象会被用户操作
   * 写入数据时向其注册写事件*/
  const Channel &channel_;
  /* 作为配置对象传递 回调函数等一些配置信息 */
  const TcpServer &server_;
};
}  // namespace skunk
}  // namespace zoo
#endif