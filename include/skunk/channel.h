#ifndef _ZOO_SKUNK_CHANNEL_H_
#define _ZOO_SKUNK_CHANNEL_H_

#include "channel.h"
#include "connection.h"
#include "tcp_server.h"

namespace zoo {
namespace skunk {
/**
 * 封装异步事件的通信
 */
class Channel final {
 public:
  using Ptr = std::shared_ptr<Channel>;
  /**
   * channel 将 Connection 与 event loop 相连接
   * event loop 是 事件源
   * connection 提供对事件处理时会话的保持
   */
  Channel(const Connection &conn, const EventLoop &loop);
  ~Channel();

  /**
   *  开启与关闭 当前channel所关心的事件集合
   */
  void EnableAccept();
  void EnableRead();
  void EnableWrite();
  void DisableAccept();
  void DisableRead();
  void DisableWrite();
  void DisableAll();

  /**
   *  返回事件源的fd
   */
  int32_t Fd() const;

  /**
   * 用于根据 event_flag_ 确定回调哪个用户函数
   */
  void HandleEvent();

 private:
  /**
   * 标记当前channel 关心的事件集合
   */
  int32_t event_flag_;

  /**
   * 携带当前进程用户注册的各种回调函数
   */
  const TcpServer &server_;

  /**
   * 提供当前tcp连接会话的读写buffer,作为回调函数的参数传入。
   */
  Connection::Ptr conn_;

  /**
   * 提供事件源, 当关注的fd上有事件到来时 找到fd对应的channel指针 并调用
   * HandleEvent 函数
   */
  const EventLoop &loop_;

  /**
   * 调用 tcp server中的回调函数 处理不同的事件逻辑
   */
  void ProcessorAccept();
  void ProcessorRead();
  void ProcessorWrite();
  void ProcessorError();
};
}  // namespace skunk
}  // namespace zoo
#endif  //_ZOO_SKUNK_CHANNEL_H_